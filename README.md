# weather-ticker
An ESP32 S3 weather ticker

## Overview

This project implements a weather ticker for ESP32-S3 with Wi-Fi configuration and SoftAP fallback support.

## Features

- **Wi-Fi Configuration**: Automatic connection to saved Wi-Fi networks
- **SoftAP Fallback**: Automatically starts a SoftAP (Access Point) if no saved credentials are found or connection fails
- **NVS Storage**: Persistent storage of Wi-Fi credentials and SoftAP settings
- **Random Password Generation**: Base32-encoded random passwords for SoftAP mode
- **Serial Console Output**: SoftAP credentials are printed to the serial console for easy access

## Wi-Fi Behavior

1. On first boot, the device will start in SoftAP mode with:
   - SSID: `Weather` (default, stored in NVS)
   - Password: Random 12-character base32 string (e.g., `A7B2C9D4E5F6`)
   - The credentials are printed to the serial console

2. Once Wi-Fi credentials are configured (future feature via web interface), they are saved to NVS

3. On subsequent boots, the device will:
   - Try to connect to the saved Wi-Fi network
   - Fall back to SoftAP mode if connection fails

## Building

This project requires ESP-IDF v5.0 or later.

### Prerequisites

1. Install ESP-IDF by following the [official guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/)
2. Set up the ESP-IDF environment:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

### Build Steps

```bash
# Configure the project (only needed once)
idf.py set-target esp32s3

# Build the project
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor
```

## Project Structure

```
weather-ticker/
├── main/
│   ├── main.c              # Main application entry point
│   ├── wifi_manager.c/h    # Wi-Fi management with SoftAP fallback
│   ├── nvs_storage.c/h     # NVS storage utilities
│   ├── base32.c/h          # Base32 password generator
│   └── CMakeLists.txt      # Component build configuration
├── partitions.csv          # Partition table (includes littlefs)
├── sdkconfig.defaults      # Default ESP-IDF configuration
└── CMakeLists.txt          # Project build configuration
```

## NVS Storage Keys

The following keys are used in NVS (namespace: `wifi_config`):

- `softap_ssid`: SoftAP SSID (default: "Weather")
- `softap_psk`: SoftAP password (randomly generated)
- `wifi_ssid`: Station mode Wi-Fi SSID
- `wifi_psk`: Station mode Wi-Fi password

## Future Features

- Web interface for Wi-Fi configuration (using littlefs for HTML storage)
- Weather data display
- Configuration portal accessible via SoftAP mode

## License

See LICENSE file for details.

