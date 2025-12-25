#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Possible commands
typedef enum {
    UI_CMD_NONE,
    UI_CMD_SHOW_AP_QR,      // Display AP QR Code
    UI_CMD_SHOW_CONFIG_QR,  // Display Configuration QR Code
    UI_CMD_SHOW_CLOCK,      // Display main screen (connected)
    UI_CMD_WIFI_CONNECTING  // Display "Connecting..." screen
} ui_cmd_t;

// The message that travels through the queue
typedef struct {
    ui_cmd_t cmd;
    char ssid[33];      // Buffer for SSID (SoftAP or Connected WiFi)
    char password[65];  // Buffer for Password (SoftAP)
} ui_msg_t;

// External declaration so wifi_manager can see the queue
extern QueueHandle_t ui_queue;
