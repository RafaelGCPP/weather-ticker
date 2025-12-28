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
void resolve_filepath(char *filepath, size_t max_len, const char *uri)
{
    // 1. Configuration App (Prefixed with /c)
    // Access: http://esp32/c -> /littlefs/config/index.html
    if (strncmp(uri, "/c", 2) == 0)
    {
        if (strlen(uri) == 2 || strcmp(uri, "/c/") == 0)
        {
            snprintf(filepath, max_len, "%s/config/index.html", MOUNT_POINT);
        }
        else
        {
            // Remove the "/c" prefix so /c/style.css becomes /config/style.css
            snprintf(filepath, max_len, "%s/config%s", MOUNT_POINT, uri + 2);
        }
    }
    // 2. Weather App (Root)
    // Access: http://esp32/ -> /littlefs/weather/index.html
    else if (strcmp(uri, "/") == 0 || strcmp(uri, "/index.html") == 0)
    {
        snprintf(filepath, max_len, "%s/weather/index.html", MOUNT_POINT);
    }
    // 3. Asset Fallback (Everything else defaults to weather app folder)
    else
    {
        snprintf(filepath, max_len, "%s/weather%s", MOUNT_POINT, uri);
    }
}

// --- HELPER 1: SET CONTENT TYPE ---
// Sets the MIME type based on the file extension
static void set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain"; // Default
    const char *ext = strrchr(filepath, '.');

    if (ext)
    {
        if (strcmp(ext, ".html") == 0)
            type = "text/html";
        else if (strcmp(ext, ".css") == 0)
            type = "text/css";
        else if (strcmp(ext, ".js") == 0)
            type = "application/javascript";
        else if (strcmp(ext, ".json") == 0)
            type = "application/json";
        else if (strcmp(ext, ".png") == 0)
            type = "image/png";
        else if (strcmp(ext, ".ico") == 0)
            type = "image/x-icon";
        else if (strcmp(ext, ".svg") == 0)
            type = "image/svg+xml";
    }
    httpd_resp_set_type(req, type);
}

// --- HELPER 2: STREAM FILE ---
// Reads the file in small chunks and sends it to the socket.
// Uses stack buffer to avoid heap fragmentation (malloc).
#define FILE_CHUNK_SIZE 1024

static esp_err_t stream_file_to_socket(httpd_req_t *req, FILE *fd)
{
    char chunk[FILE_CHUNK_SIZE]; // Allocated on the stack (fast and safe if stack > 4KB)
    size_t chunksize;
    esp_err_t err = ESP_OK;

    do
    {
        // 1. Read a chunk from LittleFS
        chunksize = fread(chunk, 1, sizeof(chunk), fd);

        if (chunksize > 0)
        {
            // 2. Send to the network
            // ESP-IDF handles EAGAIN internally here (retries)
            err = httpd_resp_send_chunk(req, chunk, chunksize);

            // If it fails (e.g., client disconnected), stop immediately
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "File send failed: %d", err);
                return err;
            }
        }
    } while (chunksize != 0);

    // 3. Finish sending (empty chunk)
    httpd_resp_send_chunk(req, NULL, 0);
    return ESP_OK;
}

// --- MAIN HANDLER ---
// Orchestrates the request
static esp_err_t common_get_handler(httpd_req_t *req)
{
    char filepath[600];

    // 1. Resolve which file the user wants (routing logic /c vs /)
    resolve_filepath(filepath, sizeof(filepath), req->uri);

    // 2. Check if the file exists (stat)
    struct stat st;
    if (stat(filepath, &st) == -1)
    {
        ESP_LOGW(TAG, "404 Not Found: %s", filepath);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    // 3. Open the file
    FILE *fd = fopen(filepath, "r");
    if (!fd)
    {
        ESP_LOGE(TAG, "500 Failed to open: %s", filepath);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    // 4. Set headers
    set_content_type_from_file(req, filepath);

    ESP_LOGI(TAG, ">>> Start Sending: %s (%ld bytes)", filepath, st.st_size);

    // --- SMART CACHE LOGIC ---

    // If it's the main file (index.html), NEVER cache.
    // The browser needs to read it to discover new .js/.css files.
    if (strstr(filepath, "index.html"))
    {
        httpd_resp_set_hdr(req, "Cache-Control", "no-cache, no-store, must-revalidate");
        httpd_resp_set_hdr(req, "Pragma", "no-cache");
        httpd_resp_set_hdr(req, "Expires", "0");
    }

    else if (strstr(filepath, "_app/"))
    {
        // Max-age: 1 year (31536000 seconds)
        // Immutable: Tells the browser the file really never changes
        httpd_resp_set_hdr(req, "Cache-Control", "max-age=31536000, immutable");
    }

    // 5. Send the content (stream)
    esp_err_t ret = stream_file_to_socket(req, fd);

    // 6. Cleanup
    fclose(fd);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "XXX Failed Sending: %s (Error: %d)", filepath, ret);
    }

    return ret;
}

// --- INITIALIZATION ---

void web_server_init(void)
{
    esp_vfs_littlefs_conf_t conf = {
        .base_path = MOUNT_POINT,
        .partition_label = "littlefs",
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    if (esp_vfs_littlefs_register(&conf) != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to mount LittleFS");
        return;
    }

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.max_uri_handlers = 10; // Increased just in case
    config.stack_size = 8192;
    config.send_wait_timeout = 60;
    config.recv_wait_timeout = 60;
    config.lru_purge_enable = true;
    ESP_LOGI(TAG, "Starting Web Server...");
    if (httpd_start(&server, &config) == ESP_OK)
    {

        // 1. API: Scan Networks
        httpd_uri_t uri_scan = {
            .uri = "/api/scan", .method = HTTP_GET, .handler = api_scan_get_handler, .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_scan);

        // 2. API: Get Configuration
        httpd_uri_t uri_config = {
            .uri = "/api/config", .method = HTTP_GET, .handler = api_config_get_handler, .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_config);

        // 3. API: Save Configuration
        httpd_uri_t uri_save = {
            .uri = "/api/save", .method = HTTP_POST, .handler = api_save_post_handler, .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_save);

        // 4. Files: Catch-All (Must be last)
        httpd_uri_t uri_files = {
            .uri = "/*", .method = HTTP_GET, .handler = common_get_handler, .user_ctx = NULL};
        httpd_register_uri_handler(server, &uri_files);
    }
}