#include "wifi_scan.h"
#include "esp_wifi.h"
#include "esp_log.h"

static const char *TAG_SCAN = "WIFI_SCAN";

void wifi_scan_ssids_json(char *output, size_t max_len)
{
    // 1. Check current mode
    wifi_mode_t current_mode;
    esp_wifi_get_mode(&current_mode);

    // 2. If we are in AP-only mode, we MUST switch to APSTA to scan
    if (current_mode == WIFI_MODE_AP) {
        ESP_LOGI(TAG_SCAN, "Switching to APSTA mode for scanning...");
        esp_wifi_set_mode(WIFI_MODE_APSTA);
        // Small delay to let the mode switch settle
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // 3. Configure the scan
    wifi_scan_config_t scan_config = {
        .ssid = NULL,
        .bssid = NULL,
        .channel = 0,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE
    };

    // 4. Start Scan (Blocking Mode)
    // Note: This blocks the HTTP Task. Ensure your Watchdog timeout is high enough
    ESP_LOGI(TAG_SCAN, "Starting Scan...");
    esp_err_t err = esp_wifi_scan_start(&scan_config, true);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG_SCAN, "Scan failed! Error: %s", esp_err_to_name(err));
        // Return empty array on error
        snprintf(output, max_len, "[]");
        // Restore mode if we changed it
        if (current_mode == WIFI_MODE_AP) esp_wifi_set_mode(WIFI_MODE_AP);
        return;
    }

    // 5. Get Records
    uint16_t ap_num = 32;
    wifi_ap_record_t ap_records[32];
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));
    
    ESP_LOGI(TAG_SCAN, "Found %d networks", ap_num);

    // 6. Build JSON
    size_t offset = 0;
    offset += snprintf(output + offset, max_len - offset, "[");

    for (int i = 0; i < ap_num; i++)
    {
        // Filter out empty SSIDs if necessary
        if (strlen((char *)ap_records[i].ssid) > 0) {
            // Check buffer limits to avoid overflow
            if (max_len - offset < 100) break; 

            offset += snprintf(output + offset, max_len - offset,
                            "{\"ssid\":\"%s\",\"rssi\":%d,\"auth\":%d}%s",
                            ap_records[i].ssid,
                            ap_records[i].rssi,
                            ap_records[i].authmode,
                            (i == ap_num - 1) ? "" : ",");
        }
    }
    
    // Handle trailing comma if the last element was skipped or empty
    if (output[offset-1] == ',') {
        offset--; // Move back to overwrite the comma
    }

    offset += snprintf(output + offset, max_len - offset, "]");

    // 7. Restore Mode (Optional, usually fine to stay in APSTA)
    // if (current_mode == WIFI_MODE_AP) {
    //     esp_wifi_set_mode(WIFI_MODE_AP);
    // }
}