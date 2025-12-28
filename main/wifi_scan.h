#pragma once
#include <stddef.h>
/**
 * @brief Scan for available Wi-Fi SSIDs and return as JSON array
 * 
 * @param output Buffer to store the JSON string
 * @param max_len Maximum length of the buffer
 */
void wifi_scan_ssids_json(char *output, size_t max_len);
