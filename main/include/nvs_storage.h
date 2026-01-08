#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include "typedefs.h"
#include <stdbool.h>
#include <stddef.h>
#include "cJSON.h"

#define NVS_NAMESPACE "wifi_config"
#define NVS_KEY_SOFTAP_SSID "softap_ssid"
#define NVS_KEY_SOFTAP_PSK "softap_psk"
#define NVS_KEY_WIFI_SSID "wifi_ssid"
#define NVS_KEY_WIFI_PSK "wifi_psk"
#define NVS_KEY_WEATHER_CITY "weather_city"
#define NVS_KEY_TIME_ZONE "time_zone"
#define NVS_KEY_NTP_SERVER "ntp_server"
#define NVS_KEY_OPENWEATHER_API_KEY "ow_api_key"


#define DEFAULT_SOFTAP_SSID "Weather"
#define SOFTAP_PSK_LENGTH 12

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

/**
 * @brief Get Weather City from NVS
 * 
 * @param city Buffer to store the city name
 * @param max_len Maximum length of the buffer
 * @return true if city was read, false if not found or error
 */
bool nvs_get_weather_city(char *city, size_t max_len);

/**
 * @brief Set Weather City in NVS
 * 
 * @param city City name to store
 * @return true on success, false on failure
 */
bool nvs_set_weather_city(const char *city);

/**
 * @brief Get Time Zone from NVS
 * 
 * @param time_zone Buffer to store the time zone
 * @param max_len Maximum length of the buffer
 * @return true if time zone was read, false if not found or error
 */
bool nvs_get_time_zone(char *time_zone, size_t max_len);

/**
 * @brief Set Time Zone in NVS
 * 
 * @param time_zone Time zone to store
 * @return true on success, false on failure
 */
bool nvs_set_time_zone(const char *time_zone);

/**
 * @brief Get Time Zone from NVS
 * 
 * @param time_zone Buffer to store the time zone
 * @param max_len Maximum length of the buffer
 * @return true if time zone was read, false if not found or error
 */
bool nvs_get_ntp_server(char *ntp_server, size_t max_len);

/**
 * @brief Set Time Zone in NVS
 * 
 * @param time_zone Time zone to store
 * @return true on success, false on failure
 */
bool nvs_set_ntp_server(const char *ntp_server);

/**
 * @brief Get OpenWeather API Key from NVS
 * 
 * @param api_key Buffer to store the API key
 * @param max_len Maximum length of the buffer
 * @return true if API key was read, false if not found or error
 */
bool nvs_get_openweather_api_key(char *api_key, size_t max_len);

/**
 * @brief Set OpenWeather API Key in NVS
 * 
 * @param api_key API key to store
 * @return true on success, false on failure
 */
bool nvs_set_openweather_api_key(const char *api_key);

/**
 * @brief Retrieve SoftAP WPA information from NVS
 * 
 * @param wpa_info Pointer to wpa_info_t structure to fill
 */
void get_AP_wpa_info_from_nvs(wpa_info_t *wpa_info);

/**
 * @brief Get full configuration from NVS as JSON string
 * 
 * @param output Buffer to store the JSON string
 * @param max_len Maximum length of the buffer
 */
void get_config_from_nvs(char *output, size_t max_len);


/**
 * @brief Save configuration from JSON object to NVS
 * 
 * @param root cJSON object containing the configuration
 */
void nvs_save_config_from_json(const cJSON *root);

#endif // NVS_STORAGE_H
