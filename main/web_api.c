#include "typedefs.h"
#include "web_api.h"
#include "esp_log.h"
#include "nvs_storage.h"
#include "wifi_scan.h"

static const char *TAG = "WEB_API";

// --- API HANDLERS (Matches Svelte Fetch Calls) ---

// GET /api/scan -> Returns available Wi-Fi networks
esp_err_t api_scan_get_handler(httpd_req_t *req) {
    // Allocate a sufficiently large buffer for JSON response
    char *json_response = heap_caps_malloc(3192, MALLOC_CAP_8BIT);
    wifi_scan_ssids_json(json_response, 3192);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
    heap_caps_free(json_response);
    return ESP_OK;
}

// GET /api/config -> Returns current settings to populate the form
esp_err_t api_config_get_handler(httpd_req_t *req) {

    char json_response[1024];
    get_config_from_nvs(json_response, sizeof(json_response));

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_response, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


// POST /api/save -> Receives JSON to save settings
esp_err_t api_save_post_handler(httpd_req_t *req) {
    char buf[512]; // Increased buffer for full config JSON
    int ret, remaining = req->content_len;

    if (remaining >= sizeof(buf)) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0';

    ESP_LOGI(TAG, "Saving Config: %s", buf);

    // TODO: Use cJSON_Parse(buf) and save to NVS here
    
    httpd_resp_send(req, "{\"status\":\"saved\"}", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
