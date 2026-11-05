/*
-------------------------------------------------------------------------------
Name:     emitter/main/emitter.cpp
Purpose:  Emisor ESP-NOW (placa ESP 01, MAC 14:C1:9F:26:D1:38).
          Cada 5 segundos envía a la placa ESP 02 (MAC E8:3D:C1:F2:AC:3C)
          un mensaje con un número de secuencia que se incrementa en cada envío.
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
#include "freertos/task.h"
#include "nvs_flash.h"

// Variables globales
static const char *TAG = "emitter";

static constexpr uint32_t PERIODO_MS = 5000;    // Tiempo entre envíos
static constexpr uint8_t CANAL_WIFI = 1;        // Debe coincidir con el del receptor

// MAC Wi-Fi STA de la placa ESP 02 (receptor)
static constexpr uint8_t MAC_RECEPTOR[ESP_NOW_ETH_ALEN] = {0xE8, 0x3D, 0xC1, 0xF2, 0xAC, 0x3C};

// Mensaje que viaja por ESP-NOW. Debe ser idéntico en emitter.cpp y receiver.cpp
struct __attribute__((packed)) Mensaje {
    uint32_t secuencia;    // Número de envío, empieza en 1
};

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
 * @brief  Callback de ESP-NOW que se ejecuta al terminar cada envío.
 *
 * Indica si el receptor confirmó la recepción (ACK de la capa MAC). Se ejecuta
 * en la tarea del Wi-Fi, así que solo registra el resultado en el log.
 *
 * @param[in]  info    Datos del envío, entre ellos la MAC de destino.
 * @param[in]  estado  ESP_NOW_SEND_SUCCESS si el receptor confirmó la recepción.
 */
static void al_enviar(const esp_now_send_info_t *info, esp_now_send_status_t estado)
{
    if (estado == ESP_NOW_SEND_SUCCESS) {
        ESP_LOGI(TAG, "Entregado a " MACSTR, MAC2STR(info->des_addr));
    } else {
        ESP_LOGW(TAG, "Sin confirmación de " MACSTR, MAC2STR(info->des_addr));
    }
}

/**
 * @brief  Inicializa ESP-NOW y registra la placa ESP 02 como destino (peer).
 */
static void iniciar_esp_now()
{
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(al_enviar));

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, MAC_RECEPTOR, ESP_NOW_ETH_ALEN);
    peer.channel = CANAL_WIFI;
    peer.ifidx = WIFI_IF_STA;
    peer.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peer));
}

/**
 * @brief  Punto de entrada de ESP-IDF.
 *
 * Inicializa el Wi-Fi y ESP-NOW y, en un bucle infinito, envía un Mensaje a
 * la placa ESP 02 cada PERIODO_MS milisegundos. No retorna nunca.
 */
extern "C" void app_main(void)
{
    iniciar_wifi();
    iniciar_esp_now();

    uint8_t mac_propia[6];
    ESP_ERROR_CHECK(esp_read_mac(mac_propia, ESP_MAC_WIFI_STA));
    ESP_LOGI(TAG, "Emisor " MACSTR " -> receptor " MACSTR " (canal %d)",
             MAC2STR(mac_propia), MAC2STR(MAC_RECEPTOR), CANAL_WIFI);

    Mensaje mensaje = {};
    while (true) {
        mensaje.secuencia++;
        esp_err_t err = esp_now_send(MAC_RECEPTOR, reinterpret_cast<const uint8_t *>(&mensaje),
                                     sizeof(mensaje));
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Enviado mensaje #%" PRIu32, mensaje.secuencia);
        } else {
            ESP_LOGE(TAG, "Error al enviar mensaje #%" PRIu32 ": %s", mensaje.secuencia,
                     esp_err_to_name(err));
        }
        vTaskDelay(pdMS_TO_TICKS(PERIODO_MS));
    }
}
