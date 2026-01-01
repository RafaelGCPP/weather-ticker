#pragma once
#include <esp_lvgl_port.h>

void show_connecting_message(const char *ssid);

void show_AP_qrcode(const char *ssid, const char *password);
void show_config_qrcode(const char *url);
void show_qrcode();

void setup_clock_screen(void);
void setup_clock_panel(lv_obj_t *parent);
void setup_weather_panel(lv_obj_t *parent);
void setup_precipitation_panel(lv_obj_t *parent);

void tick_clock(void);

extern bool s_show_clock_mode;
extern int s_last_detected_second;

extern lv_obj_t *label_hhmmss;
extern lv_obj_t *label_weekday;
extern lv_obj_t *label_date;

extern lv_obj_t *precipitation_chart;
extern lv_chart_series_t *precipitation_series;

extern lv_obj_t *right_label;
