idf_component_register(SRCS "t_display_s3.c"
        INCLUDE_DIRS "."
        REQUIRES esp_lvgl_port esp_driver_gpio esp_driver_ledc freertos esp_lcd lvgl esp_timer soc esp_adc)

