#include "esp_log.h"
#include "http_buffer_download.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"


static const int HTTP_TIMEOUT_MS = 30000; 
static const char *TAG = "HTTP_BUFFER_DOWNLOAD";

esp_err_t http_download_buffer(const char *url, char **out_buffer, size_t buffer_size, char **headers)
{
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = HTTP_TIMEOUT_MS, 
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .buffer_size = 2048, // internal buffer
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Error initializing HTTP client");
        return ESP_FAIL;
    }

    esp_err_t err;

    if ((err = esp_http_client_open(client, 0)) != ESP_OK) {
        ESP_LOGE(TAG, "Connection error: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return err;
    }

    int content_length = esp_http_client_fetch_headers(client); 
    if (content_length > buffer_size - 1) {
        ESP_LOGW(TAG, "Content-Length (%d) exceeds buffer size (%d). Data may be truncated.", 
                 content_length, buffer_size - 1);
    }
    int status_code = esp_http_client_get_status_code(client);

    err = ESP_OK;
    if (status_code >= 200 && status_code < 300) {
        
        int total_bytes_read = 0;
        int read_len;

        while (1) {
            read_len = esp_http_client_read(client, 
                                            *out_buffer + total_bytes_read, 
                                            buffer_size - total_bytes_read - 1);
            
            if (read_len < 0) {
                ESP_LOGE(TAG, "I/O error during download");
                err = ESP_FAIL;
                break;
            } 
            else if (read_len == 0) {
                break;
            }

            total_bytes_read += read_len;

            if (total_bytes_read >= buffer_size - 1) {
                ESP_LOGW(TAG, "HTTP client buffer full. JSON truncated (Increase buffer if needed).");
                err = ESP_ERR_NO_MEM;
                break; 
            }
        }

        (*out_buffer)[total_bytes_read] = '\0'; 
        ESP_LOGI(TAG, "Download OK. Total: %d bytes (%.1f%% of buffer)", 
                 total_bytes_read, (float)total_bytes_read * 100.0 / buffer_size);

    } else {
        ESP_LOGE(TAG, "HTTP Error Status: %d", status_code);
        err = ESP_FAIL;
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return err;
}