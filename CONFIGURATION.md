# Weather Ticker Configuration

## Code Organization

The firmware source code in `main/` is organized into modular directories:

### Directory Structure

```
main/
├── include/              # Public headers shared across modules
│   ├── typedefs.h       # Common data structures (OpenWeatherData, etc.)
│   ├── declares.h       # LVGL font and image declarations
│   ├── display.h        # Display initialization API
│   ├── nvs_storage.h    # NVS storage API
│   ├── ntp_manager.h    # NTP time sync API
│   └── utils/           # Utility headers
│       ├── psk_generator.h
│       └── url_encode.h
│
├── ui/                  # Display and User Interface Module
│   ├── ui.c/.h         # Main UI controller
│   ├── ui_clock.c      # Clock screen
│   ├── ui_weather.c    # Weather display
│   ├── ui_precipitation.c  # Precipitation chart
│   ├── ui_qrcode.c     # QR code display
│   ├── ui_events.c/.h  # UI event handling
│   ├── ui_internal.h   # Private UI definitions
│   ├── display.c       # Display hardware initialization
│   └── weather_utils.c/.h  # Weather formatting utilities
│
├── openweather/         # OpenWeather API Module
│   ├── openweather_service.c/.h    # Public service API
│   ├── openweather_client.c        # HTTP client
│   ├── openweather_parser.c        # JSON parser
│   └── openweather_internal.h      # Private module definitions
│
├── network/             # Network and Web Module
│   ├── wifi_manager.c/.h           # Wi-Fi connection manager
│   ├── wifi_scan.c/.h              # Wi-Fi scanning
│   ├── web_server.c/.h             # HTTP server
│   ├── web_api.c/.h                # REST API handlers
│   └── http_buffer_download.c/.h   # HTTP download utility
│
├── core/                # Core Services
│   ├── nvs_storage.c    # NVS persistence implementation
│   ├── ntp_manager.c    # NTP time synchronization
│   ├── psk_generator.c  # Random password generation
│   └── url_encode.c     # URL encoding utility
│
├── fonts/               # LVGL font files (auto-generated)
├── images/              # LVGL image files (auto-generated)
└── main.c              # Application entry point
```

### Module Dependencies

- **ui/** depends on: `include/`, `openweather/`
- **openweather/** depends on: `include/`, `network/http_buffer_download`, `core/nvs_storage`
- **network/** depends on: `include/`, `core/`
- **core/** is mostly independent, uses `include/`

### Header Visibility

- **Public headers** in `include/`: Available to all modules
- **Module headers** (e.g., `ui.h`, `openweather_service.h`): Public module APIs
- **Internal headers** (e.g., `ui_internal.h`, `openweather_internal.h`): Private to module

## Serial Console Output Example

When the device starts in SoftAP mode, you'll see output like this:

```
I (1234) wifi_manager: Initializing Wi-Fi Manager
I (1235) wifi_manager: No saved Wi-Fi credentials found, starting SoftAP
I (1240) wifi_manager: 
I (1241) wifi_manager: ===========================================
I (1242) wifi_manager:   SoftAP Mode Active
I (1243) wifi_manager: ===========================================
I (1244) wifi_manager:   SSID: Weather
I (1245) wifi_manager:   PSK:  A7B2C9D4E5F6
I (1246) wifi_manager: ===========================================
I (1247) wifi_manager: 
I (1248) wifi_manager: SoftAP started successfully
I (1249) main: System initialized successfully
```

## Connecting to the SoftAP

1. Look for a Wi-Fi network named "Weather" (or your custom SSID)
2. Connect using the password shown in the serial console
3. Once connected, you'll be able to access the configuration interface (future feature)

## Notes

- The SoftAP password is randomly generated on first boot and saved to NVS
- The password will remain the same across reboots unless you erase the NVS partition
- To reset the SoftAP credentials, erase the NVS partition using: `idf.py erase-flash`
