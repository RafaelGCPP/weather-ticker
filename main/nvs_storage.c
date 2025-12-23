#include "nvs_storage.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "nvs_storage";

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
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    size_t required_size = max_len;
    err = nvs_get_str(handle, NVS_KEY_SOFTAP_SSID, ssid, &required_size);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to read SoftAP SSID: %s", esp_err_to_name(err));
        return false;
    }
    
    return true;
}

bool nvs_get_softap_psk(char *psk, size_t max_len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    size_t required_size = max_len;
    err = nvs_get_str(handle, NVS_KEY_SOFTAP_PSK, psk, &required_size);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to read SoftAP PSK: %s", esp_err_to_name(err));
        return false;
    }
    
    return true;
}

bool nvs_set_softap_ssid(const char *ssid) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    err = nvs_set_str(handle, NVS_KEY_SOFTAP_SSID, ssid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set SoftAP SSID: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }
    
    err = nvs_commit(handle);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit SoftAP SSID: %s", esp_err_to_name(err));
        return false;
    }
    
    ESP_LOGI(TAG, "SoftAP SSID saved to NVS");
    return true;
}

bool nvs_set_softap_psk(const char *psk) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    err = nvs_set_str(handle, NVS_KEY_SOFTAP_PSK, psk);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set SoftAP PSK: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }
    
    err = nvs_commit(handle);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit SoftAP PSK: %s", esp_err_to_name(err));
        return false;
    }
    
    ESP_LOGI(TAG, "SoftAP PSK saved to NVS");
    return true;
}

bool nvs_get_wifi_ssid(char *ssid, size_t max_len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    size_t required_size = max_len;
    err = nvs_get_str(handle, NVS_KEY_WIFI_SSID, ssid, &required_size);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to read Wi-Fi SSID: %s", esp_err_to_name(err));
        return false;
    }
    
    return true;
}

bool nvs_get_wifi_psk(char *psk, size_t max_len) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    size_t required_size = max_len;
    err = nvs_get_str(handle, NVS_KEY_WIFI_PSK, psk, &required_size);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to read Wi-Fi PSK: %s", esp_err_to_name(err));
        return false;
    }
    
    return true;
}

bool nvs_set_wifi_ssid(const char *ssid) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    err = nvs_set_str(handle, NVS_KEY_WIFI_SSID, ssid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi SSID: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }
    
    err = nvs_commit(handle);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit Wi-Fi SSID: %s", esp_err_to_name(err));
        return false;
    }
    
    ESP_LOGI(TAG, "Wi-Fi SSID saved to NVS");
    return true;
}

bool nvs_set_wifi_psk(const char *psk) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS namespace: %s", esp_err_to_name(err));
        return false;
    }
    
    err = nvs_set_str(handle, NVS_KEY_WIFI_PSK, psk);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set Wi-Fi PSK: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }
    
    err = nvs_commit(handle);
    nvs_close(handle);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit Wi-Fi PSK: %s", esp_err_to_name(err));
        return false;
    }
    
    ESP_LOGI(TAG, "Wi-Fi PSK saved to NVS");
    return true;
}
