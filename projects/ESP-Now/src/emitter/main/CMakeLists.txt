idf_component_register(SRCS "emitter.cpp"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_wifi esp_netif esp_event nvs_flash log freertos)
