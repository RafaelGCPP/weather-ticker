#include "wifi_manager.h"
#include "nvs_storage.h"
#include "base32.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <string.h>

static const char *TAG = "wifi_manager";

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define MAX_RETRY 5

static EventGroupHandle_t s_wifi_event_group = NULL;
static int s_retry_num = 0;
static bool s_is_softap_mode = false;

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < MAX_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "Retry connecting to AP (%d/%d)", s_retry_num, MAX_RETRY);
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG, "Connection to AP failed");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG, "Station " MACSTR " joined, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG, "Station " MACSTR " left, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

static bool wifi_init_common(void) {
    esp_err_t ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize TCP/IP stack: %s", esp_err_to_name(ret));
        return false;
    }
    
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return false;
    }
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize Wi-Fi: %s", esp_err_to_name(ret));
        return false;
    }
    
    return true;
}

bool wifi_manager_start_softap(void) {
    char ssid[MAX_SSID_LEN] = {0};
    char psk[MAX_PSK_LEN] = {0};
    
    // Try to get SoftAP credentials from NVS
    if (!nvs_get_softap_ssid(ssid, sizeof(ssid))) {
        // Use default SSID
        strncpy(ssid, DEFAULT_SOFTAP_SSID, sizeof(ssid) - 1);
        // Save to NVS for next time
        nvs_set_softap_ssid(ssid);
    }
    
    if (!nvs_get_softap_psk(psk, sizeof(psk))) {
        // Generate random PSK
        base32_generate_random(psk, SOFTAP_PSK_LENGTH);
        // Save to NVS for next time
        nvs_set_softap_psk(psk);
    }
    
    // Print credentials to console
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "===========================================");
    ESP_LOGI(TAG, "  SoftAP Mode Active");
    ESP_LOGI(TAG, "===========================================");
    ESP_LOGI(TAG, "  SSID: %s", ssid);
    ESP_LOGI(TAG, "  PSK:  %s", psk);
    ESP_LOGI(TAG, "===========================================");
    ESP_LOGI(TAG, "");
    
    esp_netif_create_default_wifi_ap();
    
    esp_event_handler_instance_t instance_any_id;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    
    wifi_config_t wifi_config = {
        .ap = {
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    
    strncpy((char *)wifi_config.ap.ssid, ssid, sizeof(wifi_config.ap.ssid) - 1);
    strncpy((char *)wifi_config.ap.password, psk, sizeof(wifi_config.ap.password) - 1);
    wifi_config.ap.ssid_len = strlen(ssid);
    
    if (strlen(psk) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    s_is_softap_mode = true;
    ESP_LOGI(TAG, "SoftAP started successfully");
    
    return true;
}

bool wifi_manager_connect(const char *ssid, const char *password) {
    if (s_is_softap_mode) {
        ESP_LOGE(TAG, "Cannot connect to Wi-Fi while in SoftAP mode");
        return false;
    }
    
    // Create event group if not already created
    if (s_wifi_event_group == NULL) {
        s_wifi_event_group = xEventGroupCreate();
    }
    
    esp_netif_create_default_wifi_sta();
    
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    
    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
    strncpy((char *)wifi_config.sta.password, password, sizeof(wifi_config.sta.password) - 1);
    
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    
    ESP_LOGI(TAG, "Waiting for connection to %s...", ssid);
    
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);
    
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to AP SSID: %s", ssid);
        // Save credentials to NVS
        nvs_set_wifi_ssid(ssid);
        nvs_set_wifi_psk(password);
        return true;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID: %s", ssid);
        return false;
    } else {
        ESP_LOGE(TAG, "Unexpected event");
        return false;
    }
}

bool wifi_manager_init(void) {
    ESP_LOGI(TAG, "Initializing Wi-Fi Manager");
    
    if (!wifi_init_common()) {
        return false;
    }
    
    // Try to get saved Wi-Fi credentials
    char wifi_ssid[MAX_SSID_LEN] = {0};
    char wifi_psk[MAX_PSK_LEN] = {0};
    
    if (nvs_get_wifi_ssid(wifi_ssid, sizeof(wifi_ssid)) &&
        nvs_get_wifi_psk(wifi_psk, sizeof(wifi_psk))) {
        ESP_LOGI(TAG, "Found saved Wi-Fi credentials, attempting to connect...");
        
        if (wifi_manager_connect(wifi_ssid, wifi_psk)) {
            ESP_LOGI(TAG, "Successfully connected to saved Wi-Fi network");
            return true;
        }
        
        ESP_LOGW(TAG, "Failed to connect to saved Wi-Fi network, falling back to SoftAP");
    } else {
        ESP_LOGI(TAG, "No saved Wi-Fi credentials found, starting SoftAP");
    }
    
    // Fall back to SoftAP mode
    return wifi_manager_start_softap();
}
