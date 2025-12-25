#include "web_server.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_littlefs.h"
#include "esp_vfs.h"
#include "cJSON.h"
#include <sys/stat.h>

static const char *TAG = "WEB_SERVER";
#define MOUNT_POINT "/littlefs"

// --- API STUBS (Where the magic will happen later) ---

// GET /api/status -> Returns JSON with Dashboard data
static esp_err_t api_status_get_handler(httpd_req_t *req) {
    // Example JSON response
    const char *resp_str = "{\"time\": \"12:34\", \"temp\": 24.5, \"weather\": \"cloudy\", \"gps_lat\": -23.55, \"gps_lon\": -46.63}";
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// POST /api/config -> Receives Configuration JSON
static esp_err_t api_config_post_handler(httpd_req_t *req) {
    char buf[200];
    int ret, remaining = req->content_len;

    // Read POST body (JSON)
    if (remaining >= sizeof(buf)) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0) return ESP_FAIL;
    buf[ret] = '\0'; // Null-terminate

    ESP_LOGI(TAG, "Received config JSON: %s", buf);

    // TODO: Parse JSON and save to NVS
    // cJSON *root = cJSON_Parse(buf);
    // ... logic to save SSID, PSK, APIKey, GPS Params ...
    
    httpd_resp_send(req, "{\"status\":\"saved\"}", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// --- GENERIC FILE SERVER ---

// Helper function to determine Content-Type by extension
static const char* get_content_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "text/plain";
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".ico") == 0) return "image/x-icon";
    return "text/plain";
}

// Generic GET handler (Static files)
static esp_err_t common_get_handler(httpd_req_t *req) {
    char filepath[600];
    
    // Route Mapping
    if (strcmp(req->uri, "/") == 0) {
        strcpy(filepath, MOUNT_POINT "/index.html");
    } else if (strcmp(req->uri, "/c") == 0) {
        strcpy(filepath, MOUNT_POINT "/config.html");
    } else {
        // For anything else, try to find the file in LittleFS
        // Ex: /style.css -> /littlefs/style.css
        snprintf(filepath, sizeof(filepath), "%s%s", MOUNT_POINT, req->uri);
    }

    // Try to open the file
    struct stat st;
    if (stat(filepath, &st) == -1) {
        ESP_LOGE(TAG, "File not found: %s", filepath);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    FILE *fd = fopen(filepath, "r");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to read file: %s", filepath);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // Set content type
    httpd_resp_set_type(req, get_content_type(filepath));

    // Read in chunks and send (to avoid RAM overflow)
    char *chunk = malloc(1024);
    size_t chunksize;
    do {
        chunksize = fread(chunk, 1, 1024, fd);
        if (chunksize > 0) {
            httpd_resp_send_chunk(req, chunk, chunksize);
        }
    } while (chunksize != 0);

    // End the response
    httpd_resp_send_chunk(req, NULL, 0);
    
    free(chunk);
    fclose(fd);
    return ESP_OK;
}

// --- INITIALIZATION ---

void web_server_init(void) {
    // 1. Mount LittleFS
    esp_vfs_littlefs_conf_t conf = {
        .base_path = MOUNT_POINT,
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount LittleFS (%s)", esp_err_to_name(ret));
        return;
    }

    size_t total = 0, used = 0;
    ret = esp_littlefs_info(conf.partition_label, &total, &used);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "LittleFS mounted! Total: %d, Used: %d", total, used);
    }

    // 2. Start HTTP Server
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard; // Allows using *
    config.server_port = 80;
    config.core_id = 0; 
    config.stack_size = 8192; 
    config.task_priority = 5;

    ESP_LOGI(TAG, "Starting server on port: %d", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        
        // Route: API Status
        httpd_uri_t uri_api_status = {
            .uri       = "/api/status",
            .method    = HTTP_GET,
            .handler   = api_status_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_api_status);

        // Route: API Config (Save)
        httpd_uri_t uri_api_config = {
            .uri       = "/api/config", // Changed from /save for standardization
            .method    = HTTP_POST,
            .handler   = api_config_post_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_api_config);

        // Route: Catch-All (Static Files)
        // Must be registered LAST so it doesn't "swallow" the APIs
        httpd_uri_t uri_files = {
            .uri       = "/*", 
            .method    = HTTP_GET,
            .handler   = common_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &uri_files);
    }
}

