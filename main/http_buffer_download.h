#pragma once
#include "esp_err.h"
#include <stddef.h>

esp_err_t http_download_buffer(const char *url, char **out_buffer, size_t buffer_size, char **headers);