/*
-------------------------------------------------------------------------------
Name:     mac-detection/main.cpp
Purpose:  ESP-NOW identifica a cada nodo por su MAC Wi-Fi en modo estación (STA).
          Este programa la lee y la imprime cada pocos segundos, también en formato
          de array de C, listo para copiarlo en el código del emisor.
Version:  v0.0.1
Author:   David Alvarez Medina aka 0xDA_bit
Created:  05/11/2026
-------------------------------------------------------------------------------
*/
#include <cinttypes>

#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

// Variables globales
static const char *TAG = "mac-detection";
static constexpr uint32_t PERIODO_MS = 5000;

/**
 * @brief  Imprime los datos de la placa: chip, revisión, núcleos, flash y PSRAM.
 *
 * Sirven para comprobar que la configuración (target, flash y PSRAM) coincide
 * con el hardware conectado.
 */
static void mostrar_placa()
{
    esp_chip_info_t chip;
    esp_chip_info(&chip);

    uint32_t flash_bytes = 0;
    ESP_ERROR_CHECK(esp_flash_get_size(nullptr, &flash_bytes));

    // 0 si la placa no tiene PSRAM o si no está activada en la configuración
    size_t psram_bytes = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);

    ESP_LOGI(TAG, "Chip: %s (revisión v%d.%d, %d núcleos)", CONFIG_IDF_TARGET,
             chip.revision / 100, chip.revision % 100, chip.cores);
    ESP_LOGI(TAG, "Flash: %" PRIu32 " MB | PSRAM disponible: %u KB",
             flash_bytes / (1024 * 1024), static_cast<unsigned>(psram_bytes / 1024));
}

/**
 * @brief  Lee la MAC Wi-Fi en modo estación (STA), la que usa ESP-NOW, y la
 *         imprime en formato legible y como array de C.
 *
 * Se calcula a partir de la MAC base grabada de fábrica en los eFuses,
 * así que se puede leer sin arrancar el Wi-Fi.
 */
static void mostrar_mac()
{
    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_read_mac(mac, ESP_MAC_WIFI_STA));

    ESP_LOGI(TAG, "MAC Wi-Fi STA (la que usa ESP-NOW): " MACSTR, MAC2STR(mac));
    ESP_LOGI(TAG, "Para el código: {0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X}",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 * @brief  Punto de entrada de ESP-IDF.
 *
 * Muestra los datos de la placa una vez y después imprime la MAC STA cada
 * PERIODO_MS milisegundos, en un bucle infinito. No retorna nunca.
 */
extern "C" void app_main(void)
{
    mostrar_placa();

    while (true) {
        mostrar_mac();
        vTaskDelay(pdMS_TO_TICKS(PERIODO_MS));
    }
}
