idf_component_register(SRCS 
                            # Main entry point
                            "main.c"
                            
                            # UI module
                            "ui/ui.c"
                            "ui/ui_events.c"
                            "ui/ui_weather.c" 
                            "ui/ui_precipitation.c" 
                            "ui/ui_clock.c" 
                            "ui/ui_qrcode.c" 
                            "ui/display.c"
                            "ui/weather_utils.c" 
                            
                            # OpenWeather module
                            "openweather/openweather_service.c" 
                            "openweather/openweather_parser.c" 
                            "openweather/openweather_client.c" 
                            
                            # Network module
                            "network/wifi_manager.c"
                            "network/wifi_scan.c" 
                            "network/web_server.c" 
                            "network/web_api.c" 
                            "network/http_buffer_download.c" 
                            
                            # Core utilities
                            "core/nvs_storage.c"
                            "core/ntp_manager.c"
                            "core/psk_generator.c"
                            "core/url_encode.c" 
                            
                            # Fonts
                            "fonts/montserrat_sb14px.c" 
                            "fonts/barlow_condensed_sb42px.c" 
                            "fonts/barlow_condensed_sb36px.c" 
                            "fonts/barlow_condensed_sb32px.c" 
                            "fonts/barlow_condensed_sb28px.c" 
                            "fonts/barlow_condensed_sb24px.c" 
                            
                            # Images
                            "images/weather_01d.c"
                            "images/weather_01n.c"
                            "images/weather_02d.c"
                            "images/weather_02n.c"
                            "images/weather_03.c"
                            "images/weather_04.c"
                            "images/weather_09.c"
                            "images/weather_10d.c"
                            "images/weather_10n.c"
                            "images/weather_11d.c"
                            "images/weather_11n.c"
                            "images/weather_13.c"
                            "images/weather_50.c"
                            "images/weather_na.c" 
                            "images/feels_like_icon.c"
                            "images/humidity_icon.c"
                            "images/pressure_icon.c"
                            "images/uvi_icon.c"
                            "images/wind_speed_icon.c"
                            "images/wind_direction_icon.c"
                            
                    INCLUDE_DIRS "." "include" "ui" "openweather" "network" "core")
