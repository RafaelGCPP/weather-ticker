#pragma once

void show_connecting_message(const char *ssid);

void show_AP_qrcode(const char *ssid, const char *password);
void show_config_qrcode(const char *url);
void show_qrcode();

void setup_clock_screen(void);
void tick_clock(void);

extern bool s_show_clock_mode;
extern int s_last_detected_second;
