#pragma once

#define MAX_SSID_LEN 32
#define MAX_PSK_LEN 64

typedef struct {
    char ssid[MAX_SSID_LEN];
    char psk[MAX_PSK_LEN];
} wpa_info_t;