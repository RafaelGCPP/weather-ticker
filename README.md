# weather-ticker
An ESP32-S3 weather ticker with a local config portal and LVGL display.

## Overview

This project runs a SoftAP fallback, serves a Svelte config UI from LittleFS, syncs time via NTP, and fetches OpenWeather data for an on-device UI.

## Features

- **Wi-Fi Manager**: STA connect with retries plus SoftAP fallback
- **SoftAP QR Codes**: Credentials and config URL are shown on the display
- **NVS Storage**: Persistent storage for Wi-Fi and app settings
- **Random Password Generation**: 12-character alphanumeric PSK generated on first boot
- **Web Config Portal**: Svelte app served from LittleFS at `/c`
- **REST API**: `/api/scan`, `/api/config`, `/api/save`
- **Time Sync**: NTP with configurable timezone and server
- **Weather Data**: OpenWeather One Call v3 fetch with LVGL display

## Wi-Fi and Config Flow

1. On first boot (or if no saved credentials), the device starts SoftAP:
   - SSID: `Weather` (default, stored in NVS)
   - PSK: Random 12-character alphanumeric string
   - Credentials are printed to the serial console and shown as a QR code

2. When a client connects to the SoftAP, the device shows a QR code for:
   - `http://192.168.4.1/c`

3. On subsequent boots, the device:
   - Tries to connect to the saved Wi-Fi network
   - Falls back to SoftAP if connection fails
   - Starts NTP after acquiring an IP

## Web UI and API

- Config UI: `http://<device-ip>/c`
- Weather web assets: `http://<device-ip>/` (served from LittleFS)

API endpoints:
- `GET /api/scan` -> list SSIDs with RSSI/auth
- `GET /api/config` -> current config JSON
- `POST /api/save` -> save config JSON to NVS

Config JSON keys:
`wifiSsid`, `wifiPassword`, `apSsid`, `apPassword`, `weatherCity`, `timeZone`, `ntpServer`, `weatherApiKey`

## Building

This project requires ESP-IDF v5.0 or later.

### Prerequisites

1. Install ESP-IDF by following the [official guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/)
2. Set up the ESP-IDF environment:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

### Firmware Build

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

### Config Page (Svelte)

See `front-src/config/README.md` for full steps. Quick path:

```bash
cd front-src/config
npm install
npm run deploy
```

This builds to `front/config`, which is packed into the `littlefs` partition during the firmware build.

## Project Structure

```
weather-ticker/
|-- main/                 # Firmware sources (Wi-Fi, web server, UI, OpenWeather)
|-- front/                # LittleFS web assets (config + weather)
|-- front-src/config/     # Svelte config UI source
|-- partitions.csv        # Partition table (includes littlefs)
|-- sdkconfig.defaults    # Default ESP-IDF configuration
`-- CMakeLists.txt        # Project build configuration
```

## NVS Storage Keys

The following keys are used in NVS (namespace: `wifi_config`):

- `softap_ssid`: SoftAP SSID
- `softap_psk`: SoftAP password
- `wifi_ssid`: Station mode Wi-Fi SSID
- `wifi_psk`: Station mode Wi-Fi password
- `weather_city`: OpenWeather city name
- `time_zone`: TZ string (e.g., `BRT3`)
- `ntp_server`: Custom NTP server hostname
- `ow_api_key`: OpenWeather API key

## Future Work

- Improve config portal UX and validation
- Add more on-device weather views
- OTA updates

## License

See LICENSE file for details.