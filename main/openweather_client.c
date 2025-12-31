#include "esp_log.h"
#include "nvs_storage.h"
#include "url_encode.h"
#include "openweather_service.h"
#include "openweather_internal.h"
#include "http_buffer_download.h"

static const char *TAG = "OPENWEATHER_CLIENT";

static const char OW_BASE_URL[] = "https://api.openweathermap.org/";
static const char ONE_CALL_API_PATH[] = "data/3.0/onecall";
static const char GEOCODING_API_PATH[] = "geo/1.0/direct";

Coordinates get_coordinates_from_geocode(const char *api_key)
{
    char location[384];
    Coordinates coord = {0};

    if (!nvs_get_weather_city(location, sizeof(location)))
    {
        ESP_LOGE(TAG, "Weather city not found in NVS");
        return coord;
    }

    url_encode(location);

    char url[512];
    char *response_buffer = malloc(4096);

    snprintf(url, sizeof(url), "%s%s?q=%s&limit=1&appid=%s",
             OW_BASE_URL, GEOCODING_API_PATH, location, api_key);

    if (http_download_buffer(url, &response_buffer, 4096, NULL) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to download geocoding data");
        free(response_buffer);
        return coord;
    }
    cJSON *json = cJSON_Parse(response_buffer);
    if (!json)
    {
        ESP_LOGE(TAG, "Failed to parse geocoding JSON");
        free(response_buffer);
        return coord;
    }

    if (!cJSON_IsArray(json) || cJSON_GetArraySize(json) == 0)
    {
        ESP_LOGE(TAG, "Geocoding JSON is not an array or is empty");
        cJSON_Delete(json);
        free(response_buffer);
        return coord;
    }
    cJSON *firstEntry = cJSON_GetArrayItem(json, 0);
    parseCoordinates(firstEntry, &coord);

    cJSON_Delete(json);
    free(response_buffer);
    return coord;
}

void fetch_and_process_weather_data(Coordinates coord, const char *api_key)
{
    // Placeholder for future implementation
    ESP_LOGI(TAG, "Fetching weather data for Lat %.8f, Lon %.8f", coord.latitude, coord.longitude);

    char url[512];
    snprintf(url, sizeof(url), "%s%s?lat=%.8f&lon=%.8f&exclude=alerts&units=metric&appid=%s",
             OW_BASE_URL, ONE_CALL_API_PATH, coord.latitude, coord.longitude, api_key);

    ESP_LOGI(TAG, "Downloading weather data from URL: %s", url);

    char *response_buffer = malloc(65536);
    if (http_download_buffer(url, &response_buffer, 65536, NULL) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to download weather data");
        free(response_buffer);
        return;
    }

    if (openweather_lock(5000))
    {
        if (parseOpenWeatherData(response_buffer, s_weather_data))
        {
            ESP_LOGI(TAG, "Weather data updated successfully");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to parse weather data");
        }

        openweather_unlock();
    }
    else
    {
        ESP_LOGE(TAG, "Failed to acquire weather mutex for processing data");
    }

    free(response_buffer);
}
