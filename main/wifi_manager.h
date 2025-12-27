#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>


/**
 * @brief Initialize and start WiFi manager
 * 
 * This function will:
 * 1. Try to connect to a saved Wi-Fi network (if available in NVS)
 * 2. Fall back to SoftAP mode if connection fails or no credentials are saved
 * 
 * @return true on success, false on failure
 */
bool wifi_manager_init(void);

/**
 * @brief Start SoftAP mode
 * 
 * @return true on success, false on failure
 */
bool wifi_manager_start_softap(void);

/**
 * @brief Connect to a Wi-Fi network
 * 
 * @param ssid SSID of the network to connect to
 * @param password Password of the network
 * @return true on success, false on failure
 */
bool wifi_manager_connect(const char *ssid, const char *password);

#endif // WIFI_MANAGER_H
