#include "nvs_storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
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
