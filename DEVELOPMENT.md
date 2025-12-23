# Developer Documentation

## Architecture Overview

The weather ticker firmware is built on ESP-IDF and follows a modular architecture:

### Components

#### 1. NVS Storage (`nvs_storage.c/h`)

Handles persistent storage of configuration data using ESP-IDF's Non-Volatile Storage (NVS) library.

**Key Functions:**
- `nvs_storage_init()`: Initialize NVS flash
- `nvs_get_softap_ssid/psk()`: Retrieve SoftAP credentials
- `nvs_set_softap_ssid/psk()`: Store SoftAP credentials
- `nvs_get_wifi_ssid/psk()`: Retrieve station mode credentials
- `nvs_set_wifi_ssid/psk()`: Store station mode credentials

**NVS Namespace:** `wifi_config`

#### 2. Base32 Generator (`base32.c/h`)

Generates random base32-encoded strings for password generation.

**Key Functions:**
- `base32_generate_random()`: Generate a random base32 string of specified length

**Character Set:** RFC 4648 base32 alphabet (A-Z, 2-7)

#### 3. Wi-Fi Manager (`wifi_manager.c/h`)

Manages Wi-Fi connectivity with automatic SoftAP fallback.

**Key Functions:**
- `wifi_manager_init()`: Initialize Wi-Fi and attempt connection
- `wifi_manager_start_softap()`: Start SoftAP mode
- `wifi_manager_connect()`: Connect to a Wi-Fi network

**Behavior:**
1. On init, checks NVS for saved Wi-Fi credentials
2. If found, attempts to connect (max 5 retries)
3. If connection fails or no credentials found, starts SoftAP mode
4. In SoftAP mode, prints credentials to console

#### 4. Main Application (`main.c`)

Application entry point that initializes all subsystems.

**Startup Sequence:**
1. Initialize NVS storage
2. Initialize Wi-Fi Manager
3. Enter main loop

## Configuration

### Partition Table

The partition table (`partitions.csv`) includes:
- NVS partition (24KB): For configuration storage
- PHY partition (4KB): For PHY calibration data
- Factory partition (1MB): For application code
- LittleFS partition (1MB): For web interface files (future use)

### Default Settings

Default settings are in `sdkconfig.defaults`:
- Target: ESP32-S3
- Wi-Fi buffers optimized for performance
- UART console at 115200 baud
- Custom partition table

## Adding New Features

### Adding Web Interface (Future)

1. Mount littlefs partition in `main.c`
2. Add HTTP server component
3. Serve HTML/CSS/JS files from littlefs
4. Implement API endpoints for Wi-Fi configuration

### Example:

```c
// In main.c
#include "esp_littlefs.h"
#include "esp_http_server.h"

// Mount littlefs
esp_vfs_littlefs_conf_t conf = {
    .base_path = "/www",
    .partition_label = "littlefs",
    .format_if_mount_failed = true,
};
esp_vfs_littlefs_register(&conf);

// Start HTTP server
httpd_handle_t server = NULL;
httpd_config_t config = HTTPD_DEFAULT_CONFIG();
httpd_start(&server, &config);
```

## Testing

### Manual Testing

1. Flash the firmware to an ESP32-S3
2. Monitor serial output: `idf.py -p /dev/ttyUSB0 monitor`
3. Verify SoftAP credentials are printed
4. Connect to the SoftAP using a phone/laptop
5. (Future) Access web interface

### Erasing Configuration

To test first-boot behavior:
```bash
idf.py erase-flash
idf.py flash monitor
```

## Memory Considerations

- NVS uses 24KB of flash
- Wi-Fi buffers use ~100KB of RAM
- Application code fits in 1MB factory partition
- Leave room for OTA updates in future

## Security Notes

- SoftAP password is 12 characters base32 (60 bits entropy)
- Passwords are stored in NVS (can be encrypted in production)
- For production, consider:
  - Enabling NVS encryption (`CONFIG_NVS_ENCRYPTION=y`)
  - Adding secure boot
  - Adding flash encryption

## Troubleshooting

### Common Issues

1. **Build fails with "IDF_PATH not set"**
   - Solution: Run `. $HOME/esp/esp-idf/export.sh`

2. **Device not connecting to Wi-Fi**
   - Check serial output for error messages
   - Verify Wi-Fi credentials in NVS
   - Check Wi-Fi signal strength

3. **SoftAP not visible**
   - Check that Wi-Fi radio is not disabled
   - Verify antenna connection on ESP32-S3 board
   - Check serial output for errors

## Code Style

- Follow ESP-IDF coding style
- Use ESP_LOGI/LOGW/LOGE for logging
- Check return values of ESP-IDF functions
- Use ESP_ERROR_CHECK for critical operations
