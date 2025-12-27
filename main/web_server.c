#include "web_server.h"
#include "web_api.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_littlefs.h"
#include "esp_vfs.h"
#include "nvs_storage.h"
#include "cJSON.h"
#include <sys/stat.h>

static const char *TAG = "WEB_SERVER";
#define MOUNT_POINT "/littlefs"

// --- HELPER: PATH ROUTING LOGIC ---
// This is the brain that maps URLs to LittleFS folders
void resolve_filepath(char *filepath, size_t max_len, const char *uri) {
    // 1. Configuration App (Prefixed with /c)
    // Access: http://esp32/c -> /littlefs/config/index.html
    if (strncmp(uri, "/c", 2) == 0) {
        if (strlen(uri) == 2 || strcmp(uri, "/c/") == 0) {
            snprintf(filepath, max_len, "%s/config/index.html", MOUNT_POINT);
        } else {
            // Remove the "/c" prefix so /c/style.css becomes /config/style.css
            snprintf(filepath, max_len, "%s/config%s", MOUNT_POINT, uri + 2);
        }
    } 
    // 2. Weather App (Root)
    // Access: http://esp32/ -> /littlefs/weather/index.html
    else if (strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0) {
        snprintf(filepath, max_len, "%s/weather/index.html", MOUNT_POINT);
    }
    // 3. Asset Fallback (Everything else defaults to weather app folder)
    else {
        snprintf(filepath, max_len, "%s/weather%s", MOUNT_POINT, uri);
    }
}


// --- GENERIC FILE SERVER ---

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

static esp_err_t common_get_handler(httpd_req_t *req) {
    char filepath[600];
    
    // Use the new routing logic
    resolve_filepath(filepath, sizeof(filepath), req->uri);

    struct stat st;
    if (stat(filepath, &st) == -1) {
        ESP_LOGW(TAG, "Not Found: %s (URI: %s)", filepath, req->uri);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    FILE *fd = fopen(filepath, "r");
    if (!fd) {
        ESP_LOGE(TAG, "Failed to read: %s", filepath);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, get_content_type(filepath));

    char *chunk = malloc(1024);
    size_t chunksize;
    do {
        chunksize = fread(chunk, 1, 1024, fd);
        if (chunksize > 0) {
            httpd_resp_send_chunk(req, chunk, chunksize);
        }
    } while (chunksize != 0);

    httpd_resp_send_chunk(req, NULL, 0);
    free(chunk);
    fclose(fd);
    return ESP_OK;
}

// --- INITIALIZATION ---

void web_server_init(void) {
    esp_vfs_littlefs_conf_t conf = {
        .base_path = MOUNT_POINT,
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    if (esp_vfs_littlefs_register(&conf) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount LittleFS");
        return;
    }

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 10; // Increased just in case
    config.stack_size = 8192;

    ESP_LOGI(TAG, "Starting Web Server...");
    if (httpd_start(&server, &config) == ESP_OK) {
        
        // 1. API: Scan Networks
        httpd_uri_t uri_scan = {
            .uri = "/api/scan", .method = HTTP_GET, .handler = api_scan_get_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_scan);

        // 2. API: Get Configuration
        httpd_uri_t uri_config = {
            .uri = "/api/config", .method = HTTP_GET, .handler = api_config_get_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_config);

        // 3. API: Save Configuration
        httpd_uri_t uri_save = {
            .uri = "/api/save", .method = HTTP_POST, .handler = api_save_post_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_save);

        // 4. Files: Catch-All (Must be last)
        httpd_uri_t uri_files = {
            .uri = "/*", .method = HTTP_GET, .handler = common_get_handler, .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &uri_files);
    }
}