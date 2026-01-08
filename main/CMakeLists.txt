idf_component_register(SRCS "main.c"

                            "openweather/openweather_service.c" 
                            "openweather/openweather_parser.c" 
                            "openweather/openweather_client.c" 

                            "nvs/nvs_storage.c"

                            "time/ntp_manager.c"

                            "netutils/url_encode.c" 
                            "netutils/http_buffer_download.c" 
                            "netutils/psk_generator.c"

                            "ui/display.c"
                            "ui/ui.c"
                            "ui/ui_events.c"
                            "ui/ui_weather.c" 
                            "ui/ui_precipitation.c" 
                            "ui/ui_clock.c" 
                            "ui/ui_qrcode.c" 
                            "ui/weather_utils.c" 

                            "ui/fonts/montserrat_sb14px.c" 
                            "ui/fonts/barlow_condensed_sb42px.c" 
                            "ui/fonts/barlow_condensed_sb36px.c" 
                            "ui/fonts/barlow_condensed_sb32px.c" 
                            "ui/fonts/barlow_condensed_sb28px.c" 
                            "ui/fonts/barlow_condensed_sb24px.c" 

                            "ui/images/weather_01d.c"
                            "ui/images/weather_01n.c"
                            "ui/images/weather_02d.c"
                            "ui/images/weather_02n.c"
                            "ui/images/weather_03.c"
                            "ui/images/weather_04.c"
                            "ui/images/weather_09.c"
                            "ui/images/weather_10d.c"
                            "ui/images/weather_10n.c"
                            "ui/images/weather_11d.c"
                            "ui/images/weather_11n.c"
                            "ui/images/weather_13.c"
                            "ui/images/weather_50.c"
                            "ui/images/weather_na.c" 
                            "ui/images/feels_like_icon.c"
                            "ui/images/humidity_icon.c"
                            "ui/images/pressure_icon.c"
                            "ui/images/uvi_icon.c"
                            "ui/images/wind_speed_icon.c"
                            "ui/images/wind_direction_icon.c"

                            "webserver/web_api.c" 
                            "webserver/web_server.c" 

                            "wifi/wifi_scan.c"                           
                            "wifi/wifi_manager.c"

                    INCLUDE_DIRS "." "include")
