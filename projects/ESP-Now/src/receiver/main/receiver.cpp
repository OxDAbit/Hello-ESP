/*
-------------------------------------------------------------------------------
Name:     receiver/main/receiver.cpp
Purpose:  Receptor ESP-NOW (placa ESP 02, MAC E8:3D:C1:F2:AC:3C).
          Espera los mensajes de la placa ESP 01 (MAC 14:C1:9F:26:D1:38) y,
          por cada uno, hace parpadear el LED RGB integrado (WS2812, GPIO48)
          una vez en rojo, otra en verde y otra en azul. Ignora los mensajes
          que lleguen de cualquier otra MAC.
Version:  v0.0.1
Author:   David Alvarez Medina aka 0xDA_bit
Created:  05/11/2026
-------------------------------------------------------------------------------
*/
#include <cinttypes>
#include <cstring>

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "nvs_flash.h"

// Variables globales
static const char *TAG = "receiver";

static constexpr uint8_t CANAL_WIFI = 1;           // Debe coincidir con el del emisor
static constexpr int GPIO_LED = 48;                // LED RGB integrado (WS2812)
static constexpr uint32_t T_ENCENDIDO_MS = 300;    // Tiempo encendido de cada parpadeo
static constexpr uint32_t T_APAGADO_MS = 500;      // Tiempo apagado de cada parpadeo
static constexpr uint8_t BRILLO = 32;              // 0-255: a 255 el WS2812 deslumbra
static constexpr UBaseType_t LONGITUD_COLA = 4;    // Mensajes pendientes de procesar

// MAC Wi-Fi STA de la placa ESP 01 (emisor): solo se aceptan sus mensajes
static constexpr uint8_t MAC_EMISOR[ESP_NOW_ETH_ALEN] = {0x14, 0xC1, 0x9F, 0x26, 0xD1, 0x38};

// Mensaje que viaja por ESP-NOW. Debe ser idéntico en emitter.cpp y receiver.cpp
struct __attribute__((packed)) Mensaje {
    uint32_t secuencia;    // Número de envío, empieza en 1
};

// Color de un parpadeo, en componentes R, G y B (0-255)
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    const char *nombre;
};

// Secuencia que se ejecuta por cada mensaje recibido: un parpadeo de cada color
static constexpr Color SECUENCIA[] = {
    {BRILLO, 0, 0, "rojo"},
    {0, BRILLO, 0, "verde"},
    {0, 0, BRILLO, "azul"},
};

// Cola por la que el callback de ESP-NOW pasa los mensajes a app_main
static QueueHandle_t cola_mensajes = nullptr;

/**
 * @brief  Inicializa la NVS, la pila de red y el Wi-Fi en modo estación (STA).
 *
 * ESP-NOW necesita el Wi-Fi arrancado, pero no conectado a ningún punto de
 * acceso. Se fija el canal CANAL_WIFI, que tiene que ser el mismo en las dos placas.
 */
static void iniciar_wifi()
{
    // El driver Wi-Fi guarda su calibración en la NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(CANAL_WIFI, WIFI_SECOND_CHAN_NONE));
}

/**
 * @brief  Callback de ESP-NOW que se ejecuta al recibir un paquete.
 *
 * Corre en la tarea del Wi-Fi, así que no puede bloquearse con los parpadeos:
 * descarta lo que no viene de MAC_EMISOR o no tiene el tamaño de un Mensaje y
 * pasa el resto a app_main a través de cola_mensajes.
 *
 * @param[in]  info   Datos de la recepción, entre ellos la MAC de origen.
 * @param[in]  datos  Contenido del paquete.
 * @param[in]  largo  Tamaño del paquete en bytes.
 */
static void al_recibir(const esp_now_recv_info_t *info, const uint8_t *datos, int largo)
{
    if (memcmp(info->src_addr, MAC_EMISOR, ESP_NOW_ETH_ALEN) != 0) {
        ESP_LOGW(TAG, "Ignorado: paquete de " MACSTR " (no es la placa ESP 01)",
                 MAC2STR(info->src_addr));
        return;
    }
    if (largo != sizeof(Mensaje)) {
        ESP_LOGW(TAG, "Ignorado: tamaño %d bytes, se esperaban %u", largo,
                 static_cast<unsigned>(sizeof(Mensaje)));
        return;
    }

    Mensaje mensaje;
    memcpy(&mensaje, datos, sizeof(mensaje));
    if (xQueueSend(cola_mensajes, &mensaje, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Cola llena: se descarta el mensaje #%" PRIu32, mensaje.secuencia);
    }
}

/**
 * @brief  Inicializa ESP-NOW y registra el callback de recepción.
 *
 * El receptor no necesita añadir al emisor como peer: solo hace falta para enviar.
 */
static void iniciar_esp_now()
{
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(al_recibir));
}

/**
 * @brief  Inicializa el LED RGB integrado (WS2812) en GPIO_LED mediante el periférico RMT.
 *
 * @return Manejador del LED, listo para usar con las funciones de led_strip.
 */
static led_strip_handle_t crear_led()
{
    led_strip_config_t config = {};
    config.strip_gpio_num = GPIO_LED;
    config.max_leds = 1;                            // La placa solo tiene un LED
    config.led_model = LED_MODEL_WS2812;
    config.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;

    led_strip_rmt_config_t rmt_config = {};
    rmt_config.clk_src = RMT_CLK_SRC_DEFAULT;
    rmt_config.resolution_hz = 10 * 1000 * 1000;    // 10 MHz: 0,1 µs por tick

    led_strip_handle_t led = nullptr;
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&config, &rmt_config, &led));
    ESP_ERROR_CHECK(led_strip_clear(led));
    return led;
}

/**
 * @brief  Hace parpadear el LED una vez en un color.
 *
 * Deja el LED encendido T_ENCENDIDO_MS y apagado T_APAGADO_MS.
 *
 * @param[in]  led    Manejador del LED devuelto por crear_led().
 * @param[in]  color  Color del parpadeo.
 */
static void parpadear(led_strip_handle_t led, const Color &color)
{
    ESP_ERROR_CHECK(led_strip_set_pixel(led, 0, color.r, color.g, color.b));
    ESP_ERROR_CHECK(led_strip_refresh(led));
    vTaskDelay(pdMS_TO_TICKS(T_ENCENDIDO_MS));

    ESP_ERROR_CHECK(led_strip_clear(led));
    vTaskDelay(pdMS_TO_TICKS(T_APAGADO_MS));
}

/**
 * @brief  Punto de entrada de ESP-IDF.
 *
 * Inicializa el LED, el Wi-Fi y ESP-NOW y, en un bucle infinito, espera los
 * mensajes de la placa ESP 01. Por cada uno ejecuta la SECUENCIA de parpadeos
 * (rojo, verde y azul). No retorna nunca.
 */
extern "C" void app_main(void)
{
    led_strip_handle_t led = crear_led();

    cola_mensajes = xQueueCreate(LONGITUD_COLA, sizeof(Mensaje));
    configASSERT(cola_mensajes != nullptr);

    iniciar_wifi();
    iniciar_esp_now();

    uint8_t mac_propia[6];
    ESP_ERROR_CHECK(esp_read_mac(mac_propia, ESP_MAC_WIFI_STA));
    ESP_LOGI(TAG, "Receptor " MACSTR " esperando a " MACSTR " (canal %d, LED en GPIO%d)",
             MAC2STR(mac_propia), MAC2STR(MAC_EMISOR), CANAL_WIFI, GPIO_LED);

    Mensaje mensaje;
    while (true) {
        if (xQueueReceive(cola_mensajes, &mensaje, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Recibido mensaje #%" PRIu32 " de la placa ESP 01", mensaje.secuencia);
            for (const Color &color : SECUENCIA) {
                parpadear(led, color);
            }
        }
    }
}
