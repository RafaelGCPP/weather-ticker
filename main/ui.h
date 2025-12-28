#pragma once


void update_ui(void);
void ui_init(void);
void ui_init_queue(void); 

void ui_show_AP_qr(const char * ssid, const char * psk);
void ui_show_config_qrcode(const char * url);
void ui_show_clock(void);