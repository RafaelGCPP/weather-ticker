#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include <stdbool.h>
#include <stddef.h>

#define NVS_NAMESPACE "wifi_config"
#define NVS_KEY_SOFTAP_SSID "softap_ssid"
#define NVS_KEY_SOFTAP_PSK "softap_psk"
#define NVS_KEY_WIFI_SSID "wifi_ssid"
#define NVS_KEY_WIFI_PSK "wifi_psk"

/**
 * @brief Initialize NVS storage
 * 
 * @return true on success, false on failure
 */
bool nvs_storage_init(void);

/**
 * @brief Get SoftAP SSID from NVS
 * 
 * @param ssid Buffer to store the SSID
 * @param max_len Maximum length of the buffer
 * @return true if SSID was read, false if not found or error
 */
bool nvs_get_softap_ssid(char *ssid, size_t max_len);

/**
 * @brief Get SoftAP PSK from NVS
 * 
 * @param psk Buffer to store the PSK
 * @param max_len Maximum length of the buffer
 * @return true if PSK was read, false if not found or error
 */
bool nvs_get_softap_psk(char *psk, size_t max_len);

/**
 * @brief Set SoftAP SSID in NVS
 * 
 * @param ssid SSID to store
 * @return true on success, false on failure
 */
bool nvs_set_softap_ssid(const char *ssid);

/**
 * @brief Set SoftAP PSK in NVS
 * 
 * @param psk PSK to store
 * @return true on success, false on failure
 */
bool nvs_set_softap_psk(const char *psk);

/**
 * @brief Get Wi-Fi SSID from NVS
 * 
 * @param ssid Buffer to store the SSID
 * @param max_len Maximum length of the buffer
 * @return true if SSID was read, false if not found or error
 */
bool nvs_get_wifi_ssid(char *ssid, size_t max_len);

/**
 * @brief Get Wi-Fi PSK from NVS
 * 
 * @param psk Buffer to store the PSK
 * @param max_len Maximum length of the buffer
 * @return true if PSK was read, false if not found or error
 */
bool nvs_get_wifi_psk(char *psk, size_t max_len);

/**
 * @brief Set Wi-Fi SSID in NVS
 * 
 * @param ssid SSID to store
 * @return true on success, false on failure
 */
bool nvs_set_wifi_ssid(const char *ssid);

/**
 * @brief Set Wi-Fi PSK in NVS
 * 
 * @param psk PSK to store
 * @return true on success, false on failure
 */
bool nvs_set_wifi_psk(const char *psk);

#endif // NVS_STORAGE_H
