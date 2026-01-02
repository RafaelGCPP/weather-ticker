#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "openweather_service.h"
#include "openweather_internal.h"
#include "nvs_storage.h"
#include "esp_log.h"

static const char *TAG = "OPENWEATHER_SERVICE";
static const int OPENWEATHER_UPDATE_INTERVAL_MS = 10 * 60 * 1000; // 10 minutes

OpenWeatherData *s_weather_data = NULL;
SemaphoreHandle_t s_weather_mutex = NULL;

void openweather_service_init()
{
    s_weather_mutex = xSemaphoreCreateMutex();
    if (s_weather_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create weather mutex");
        return;
    }
    s_weather_data = malloc(sizeof(OpenWeatherData));
    if (s_weather_data == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for weather data");
        return;
    }
    memset(s_weather_data, 0, sizeof(OpenWeatherData));

    xTaskCreatePinnedToCore(openweather_service_task, "openweather", 12288, NULL, 3, NULL, 1);
}

void openweather_service_task(void *pvParameters)
{
    char api_key[64];
    if (!nvs_get_openweather_api_key(api_key, sizeof(api_key)))
    {
        ESP_LOGE(TAG, "OpenWeather API key not found in NVS");
        vTaskDelete(NULL);
        return;
    }

    Coordinates coord = get_coordinates_from_geocode(api_key);
    while (coord.latitude == 0 && coord.longitude == 0)
    {
        int retries = 5;
        ESP_LOGW(TAG, "Failed to get coordinates from geocode, retrying in 10 seconds...");
        vTaskDelay(pdMS_TO_TICKS(10000));
        coord = get_coordinates_from_geocode(api_key);
        if (--retries <= 0)
        {
            ESP_LOGE(TAG, "Could not get valid coordinates in 5 retries, stopping weather service task");
            vTaskDelete(NULL);
            return;
        }
    }
    ESP_LOGI(TAG, "Coordinates: Lat %.4f, Lon %.4f", coord.latitude, coord.longitude);

    while (1)
    {
        fetch_and_process_weather_data(coord, api_key);
        vTaskDelay(pdMS_TO_TICKS(OPENWEATHER_UPDATE_INTERVAL_MS)); // 60 minutes
    }
}

bool openweather_lock(int timeout)
{
    if (s_weather_mutex)
    {
        return xSemaphoreTake(s_weather_mutex, pdMS_TO_TICKS(timeout));
    }
    return false;
}

void openweather_unlock()
{
    if (s_weather_mutex != NULL)
    {
        xSemaphoreGive(s_weather_mutex);
    }
}

CurrentWeather *openweather_get_current_data()
{
    return &(s_weather_data->current);
}

void openweather_get_scaled_minutely_precipitation_data(int32_t *data_out)
{
    for (int i = 0; i < NUM_MINUTELY; i++)
    {
        data_out[i] = (int32_t)(s_weather_data->minutely[i].precipitation * 60); // Scale to mm/h
    }
}