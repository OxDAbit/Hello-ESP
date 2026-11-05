/*
-------------------------------------------------------------------------------
Name:     led-onboard-ESP32-S3-N16R8/main/main.cpp
Purpose:  Parpadeo del LED RGB integrado (WS2812) de la placa ESP32-S3-N16R8.
          Secuencia en bucle: 3 parpadeos en verde, 3 en rojo y 1 en azul,
          con 300 ms encendido y 500 ms apagado en cada parpadeo.
Version:  v0.0.1
Author:   David Alvarez Medina aka 0xDA_bit
Created:  05/11/2026
-------------------------------------------------------------------------------
*/
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "sdkconfig.h"

// Variables globales
static const char *TAG = "led-onboard";

static constexpr uint32_t T_ENCENDIDO_MS = 300;    // Tiempo encendido de cada parpadeo
static constexpr uint32_t T_APAGADO_MS = 500;      // Tiempo apagado de cada parpadeo
static constexpr uint8_t BRILLO = 32;              // 0-255: a 255 el WS2812 deslumbra

// Color de un parpadeo, en componentes R, G y B (0-255)
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    const char *nombre;
};

// Un paso de la secuencia: un color y cuántas veces parpadea
struct Paso {
    Color color;
    int veces;
};

static constexpr Paso SECUENCIA[] = {
    {{0, BRILLO, 0, "verde"}, 3},
    {{BRILLO, 0, 0, "rojo"}, 3},
    {{0, 0, BRILLO, "azul"}, 1},
};

/**
 * @brief  Inicializa el LED RGB integrado (WS2812) mediante el periférico RMT.
 *
 * El pin se configura en menuconfig (Ejemplo LED integrado → GPIO del LED RGB),
 * con GPIO48 por defecto.
 *
 * @return Manejador del LED, listo para usar con las funciones de led_strip.
 */
static led_strip_handle_t crear_led()
{
    led_strip_config_t config = {};
    config.strip_gpio_num = CONFIG_LED_GPIO;
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
 * @brief  Hace parpadear el LED en un color un número de veces.
 *
 * Cada parpadeo deja el LED encendido T_ENCENDIDO_MS y apagado T_APAGADO_MS.
 *
 * @param[in]  led    Manejador del LED devuelto por crear_led().
 * @param[in]  color  Color del parpadeo.
 * @param[in]  veces  Número de parpadeos.
 */
static void parpadear(led_strip_handle_t led, const Color &color, int veces)
{
    for (int i = 0; i < veces; i++) {
        ESP_ERROR_CHECK(led_strip_set_pixel(led, 0, color.r, color.g, color.b));
        ESP_ERROR_CHECK(led_strip_refresh(led));
        vTaskDelay(pdMS_TO_TICKS(T_ENCENDIDO_MS));

        ESP_ERROR_CHECK(led_strip_clear(led));
        vTaskDelay(pdMS_TO_TICKS(T_APAGADO_MS));
    }
}

/**
 * @brief  Punto de entrada de ESP-IDF.
 *
 * Inicializa el LED y repite la secuencia (3 verde, 3 rojo, 1 azul) en un
 * bucle infinito. No retorna nunca.
 */
extern "C" void app_main(void)
{
    led_strip_handle_t led = crear_led();
    ESP_LOGI(TAG, "LED RGB en GPIO%d", CONFIG_LED_GPIO);

    while (true) {
        for (const Paso &paso : SECUENCIA) {
            ESP_LOGI(TAG, "%d x %s", paso.veces, paso.color.nombre);
            parpadear(led, paso.color, paso.veces);
        }
    }
}
