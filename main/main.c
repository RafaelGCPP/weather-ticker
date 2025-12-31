#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_storage.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "display.h"
#include "openweather_service.h"

static const char *TAG = "main";
static void check_memory_allocation();

void app_main(void)
{
    ESP_LOGI(TAG, "Weather Ticker - Starting up");
    ESP_LOGI(TAG, "Chip: %s", CONFIG_IDF_TARGET);
    check_memory_allocation();

    // Initialize Display and start Update task
    initialize_display();

    // Initialize NVS
    if (!nvs_storage_init())
    {
        ESP_LOGE(TAG, "Failed to initialize NVS storage");
        return;
    }

    // Initialize Wi-Fi Manager
    if (!wifi_manager_init())
    {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi Manager");
        return;
    }

    web_server_init();
    // Main loop

    openweather_service_init();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


static void check_memory_allocation() {
    ESP_LOGI("MEM", "Internal Free: %d bytes", heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    ESP_LOGI("MEM", "SPIRAM Free:   %d bytes", heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

    void* ptr_small = malloc(100);
    if (esp_ptr_internal(ptr_small)) {
        ESP_LOGI("TEST", "malloc(100) -> INTERNAL RAM (OK)");
    } else {
        ESP_LOGI("TEST", "malloc(100) -> PSRAM");
    }

    void* ptr_big = malloc(5000);
    if (esp_ptr_external_ram(ptr_big)) {
        ESP_LOGI("TEST", "malloc(5000) -> PSRAM (Success!)");
    } else {
        ESP_LOGW("TEST", "malloc(5000) -> INTERNAL RAM (Warning: Configuration failed)");
    }

    free(ptr_small);
    free(ptr_big);
}