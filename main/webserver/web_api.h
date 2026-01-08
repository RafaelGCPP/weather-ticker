#pragma once
#include "esp_log.h"
#include "esp_http_server.h"

esp_err_t api_scan_get_handler(httpd_req_t *req);
esp_err_t api_config_get_handler(httpd_req_t *req);
esp_err_t api_save_post_handler(httpd_req_t *req);
