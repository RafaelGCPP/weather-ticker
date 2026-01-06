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

This project requires ESP-IDF v5.0 or later and Node.js 20+ (for front-end build).

### Prerequisites

1. Install ESP-IDF by following the [official guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/)
2. Set up the ESP-IDF environment:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```
3. Node.js 20+ is required for building the front-end
   - If using the devcontainer, Node.js is pre-installed
   - Otherwise, install from [nodejs.org](https://nodejs.org/)

### Complete Build (Firmware + Front-end)

The build process now automatically builds the front-end:

```bash
# Configure the project (only needed once)
idf.py set-target esp32s3

# Build everything (firmware + front-end)
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor
```

**What happens during build:**
1. CMake triggers the front-end build target
2. `npm install` runs (if needed) in `front-src/config/`
3. `npm run deploy` builds the Svelte app and copies to `front/config/`
4. LittleFS partition image is created with the front-end files
5. ESP32 firmware is compiled and linked

See [FRONTEND_BUILD.md](FRONTEND_BUILD.md) for detailed front-end build documentation.

### Manual Front-end Development

For front-end development without rebuilding the firmware:

### Manual Front-end Development

For front-end development without rebuilding the firmware:

```bash
cd front-src/config
npm install
npm run dev         # Development server with hot reload
npm run deploy      # Build and deploy to front/config/
```

## Project Structure

```
weather-ticker/
├── main/                    # Firmware sources (reorganized into modules)
│   ├── include/            # Public headers (typedefs, APIs)
│   ├── ui/                 # Display and UI module
│   ├── openweather/        # OpenWeather API client
│   ├── network/            # Wi-Fi, web server, HTTP
│   ├── core/               # NVS, NTP, utilities
│   ├── fonts/              # LVGL fonts
│   └── images/             # LVGL images
├── front/                   # LittleFS web assets (auto-generated)
├── front-src/config/        # Svelte config UI source
├── build-frontend.sh        # Front-end build script
├── partitions.csv           # Partition table (includes littlefs)
├── sdkconfig.defaults       # Default ESP-IDF configuration
└── CMakeLists.txt          # Project build configuration
```

See [CONFIGURATION.md](CONFIGURATION.md) for code organization details.

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