#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_storage.h"
#include "wifi_manager.h"

static const char *TAG = "main";

void app_main(void) {
    ESP_LOGI(TAG, "Weather Ticker - Starting up");
    ESP_LOGI(TAG, "Chip: %s", CONFIG_IDF_TARGET);
    
    // Initialize NVS
    if (!nvs_storage_init()) {
        ESP_LOGE(TAG, "Failed to initialize NVS storage");
        return;
    }
    
    // Initialize Wi-Fi Manager
    if (!wifi_manager_init()) {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi Manager");
        return;
    }
    
    ESP_LOGI(TAG, "System initialized successfully");
    
    // Main loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
