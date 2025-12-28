#include "ntp_manager.h"
#include "esp_sntp.h"
#include "esp_log.h"
#include "nvs_storage.h" 
#include "ui.h"
#include <time.h>
#include <sys/time.h>
#include <string.h>

static const char *TAG = "NTP_MANAGER";

// IMPORTANT: Static buffer to persist in memory
// esp_sntp_setservername only stores the pointer, it does not copy the string!
static char s_custom_ntp_server[64] = {0};

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Clock synchronized via NTP!");
    ui_show_clock();
}

void ntp_manager_init(void)
{
    // 1. Configure Timezone (Read from NVS)
    char time_zone[64] = {0};
    
    // Try to read from NVS. If it fails or is empty, use default.
    if (nvs_get_time_zone(time_zone, sizeof(time_zone)) && strlen(time_zone) > 0) {
        ESP_LOGI(TAG, "Setting Timezone from NVS: %s", time_zone);
    } else {
        // Fallback: Brasília (UTC-3)
        strcpy(time_zone, "BRT3");
        ESP_LOGW(TAG, "Timezone not set in NVS. Using default: %s", time_zone);
    }
    
    // setenv makes an internal copy, so local 'time_zone' is safe here
    setenv("TZ", time_zone, 1);
    tzset();

    // 2. Check if already running
    if (esp_sntp_enabled()) {
        ESP_LOGI(TAG, "NTP is already running. Restarting to apply new settings...");
        esp_sntp_stop();
    }

    ESP_LOGI(TAG, "Initializing SNTP...");

    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    
    // 3. Configure NTP Server (From NVS)
    // Clear the static buffer before use
    memset(s_custom_ntp_server, 0, sizeof(s_custom_ntp_server));

    if (nvs_get_ntp_server(s_custom_ntp_server, sizeof(s_custom_ntp_server)) && strlen(s_custom_ntp_server) > 0) {
        ESP_LOGI(TAG, "Using Custom NTP Server: %s", s_custom_ntp_server);
        // Use the static buffer
        esp_sntp_setservername(0, s_custom_ntp_server);
    } else {
        ESP_LOGI(TAG, "Using Default NTP Server: pool.ntp.org");
        esp_sntp_setservername(0, "pool.ntp.org");
    }

    // Fallback server (always good to have)
    esp_sntp_setservername(1, "pool.ntp.org");
    esp_sntp_setservername(2, "time.google.com");


    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    
    // 10 minute interval (in ms)
    esp_sntp_set_sync_interval(NTP_SYNC_INTERVAL_MS); 

    esp_sntp_init();
}

void ntp_manager_sync(void) {
    if (esp_sntp_enabled()) {
        esp_sntp_stop();
        esp_sntp_init();
    }
}


void get_time(bool local, char *buffer, size_t max_len)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    if (local) {
        localtime_r(&now, &timeinfo);
    } else {
        gmtime_r(&now, &timeinfo);
    }
    strftime(buffer, max_len, "%c", &timeinfo);
}