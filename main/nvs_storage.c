#include "nvs_storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "psk_generator.h"
#include <string.h>

static const char *TAG = "nvs_storage";

typedef esp_err_t (*nvs_write_cb_t)(nvs_handle_t handle, const char *key, const void *value);

static esp_err_t nvs_write_str(nvs_handle_t handle, const char *key, const void *value) {
    return nvs_set_str(handle, key, (const char *)value);
}

static bool nvs_set_generic(const char *key, nvs_write_cb_t write_cb, const void *value, const char *item_name) {
    if (key == NULL || write_cb == NULL || item_name == NULL) {
        ESP_LOGE(TAG, "Invalid parameters to nvs_set_generic");
        return false;
    }
    if (value == NULL) {
        ESP_LOGE(TAG, "Failed to set %s: null value", item_name);
        return false;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }

    err = write_cb(handle, key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set %s: %s", item_name, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit %s: %s", item_name, esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "%s saved to NVS", item_name);
    return true;
}

static bool nvs_get_str_generic(const char *key, char *out, size_t max_len, const char *item_name) {
    if (key == NULL || out == NULL || item_name == NULL) {
        ESP_LOGD(TAG, "Invalid parameters to nvs_get_str_generic");
        return false;
    }
    if (max_len == 0) {
        ESP_LOGD(TAG, "Failed to read %s: buffer too small", item_name);
        return false;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }

    size_t required_size = max_len;
    err = nvs_get_str(handle, key, out, &required_size);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to read %s: %s", item_name, esp_err_to_name(err));
        return false;
    }

    return true;
}

bool nvs_storage_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated and needs to be erased");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(ret));
        return false;
    }
    
    ESP_LOGI(TAG, "NVS initialized successfully");
    return true;
}

bool nvs_get_softap_ssid(char *ssid, size_t max_len) {
    return nvs_get_str_generic(NVS_KEY_SOFTAP_SSID, ssid, max_len, "SoftAP SSID");
}

bool nvs_get_softap_psk(char *psk, size_t max_len) {
    return nvs_get_str_generic(NVS_KEY_SOFTAP_PSK, psk, max_len, "SoftAP PSK");
}

bool nvs_set_softap_ssid(const char *ssid) {
    return nvs_set_generic(NVS_KEY_SOFTAP_SSID, nvs_write_str, ssid, "SoftAP SSID");
}

bool nvs_set_softap_psk(const char *psk) {
    return nvs_set_generic(NVS_KEY_SOFTAP_PSK, nvs_write_str, psk, "SoftAP PSK");
}

bool nvs_get_wifi_ssid(char *ssid, size_t max_len) {
    return nvs_get_str_generic(NVS_KEY_WIFI_SSID, ssid, max_len, "Wi-Fi SSID");
}

bool nvs_get_wifi_psk(char *psk, size_t max_len) {
    return nvs_get_str_generic(NVS_KEY_WIFI_PSK, psk, max_len, "Wi-Fi PSK");
}

bool nvs_set_wifi_ssid(const char *ssid) {
    return nvs_set_generic(NVS_KEY_WIFI_SSID, nvs_write_str, ssid, "Wi-Fi SSID");
}

bool nvs_set_wifi_psk(const char *psk) {
    return nvs_set_generic(NVS_KEY_WIFI_PSK, nvs_write_str, psk, "Wi-Fi PSK");
}

bool nvs_get_weather_city(char *city, size_t max_len) {
    return nvs_get_str_generic(NVS_KEY_WEATHER_CITY, city, max_len, "Weather City");
}

bool nvs_get_time_zone(char *time_zone, size_t max_len) {
    return nvs_get_str_generic(NVS_KEY_TIME_ZONE, time_zone, max_len, "Time Zone");
}

void get_AP_wpa_info_from_nvs(wpa_info_t *wpa_info)
{
    if (!nvs_get_softap_ssid(wpa_info->ssid, sizeof(wpa_info->ssid)))
    {
        // Use default SSID
        strncpy(wpa_info->ssid, DEFAULT_SOFTAP_SSID, sizeof(wpa_info->ssid) - 1);
        // Save to NVS for next time
        nvs_set_softap_ssid(wpa_info->ssid);
    }

    if (!nvs_get_softap_psk(wpa_info->psk, sizeof(wpa_info->psk)))
    {
        // Generate random PSK
        generate_random_psk(wpa_info->psk, SOFTAP_PSK_LENGTH);
        // Save to NVS for next time
        nvs_set_softap_psk(wpa_info->psk);
    }
}

// function to read all stored information and format it as JSON

void get_config_from_nvs(char *output, size_t max_len)
{
    char ap_ssid[MAX_SSID_LEN] = {0};
    char ap_psk[MAX_PSK_LEN] = {0};
    char wifi_ssid[MAX_SSID_LEN] = {0};
    char wifi_psk[MAX_PSK_LEN] = {0};
    char weather_city[64] = {0};
    char time_zone[64] = {0};

    bool has_ap_ssid = nvs_get_softap_ssid(ap_ssid, sizeof(ap_ssid));
    bool has_ap_psk = nvs_get_softap_psk(ap_psk, sizeof(ap_psk));
    bool has_wifi_ssid = nvs_get_wifi_ssid(wifi_ssid, sizeof(wifi_ssid));
    bool has_wifi_psk = nvs_get_wifi_psk(wifi_psk, sizeof(wifi_psk));
    bool has_weather_city = nvs_get_weather_city(weather_city, sizeof(weather_city));
    bool has_time_zone = nvs_get_time_zone(time_zone, sizeof(time_zone));
    size_t offset = 0;
    offset += snprintf(output + offset, max_len - offset, "{");
    if (has_ap_ssid)
    {
        offset += snprintf(output + offset, max_len - offset, "\"apSsid\":\"%s\",", ap_ssid);
    }
    if (has_ap_psk)
    {
        offset += snprintf(output + offset, max_len - offset, "\"apPassword\":\"%s\",", ap_psk);
    }
    if (has_wifi_ssid)
    {
        offset += snprintf(output + offset, max_len - offset, "\"wifiSsid\":\"%s\",", wifi_ssid);
    }
    if (has_wifi_psk)
    {
        offset += snprintf(output + offset, max_len - offset, "\"wifiPassword\":\"%s\",", wifi_psk);
    }
    if (has_weather_city)
    {
        offset += snprintf(output + offset, max_len - offset, "\"weatherCity\":\"%s\",", weather_city);
    }
    if (has_time_zone)
    {
        offset += snprintf(output + offset, max_len - offset, "\"timeZone\":\"%s\",", time_zone);
    }
    if (offset > 1 && output[offset - 1] == ',') {
        output[--offset] = '\0'; // Remove trailing comma
    }
    offset += snprintf(output + offset, max_len - offset, "}");
}

