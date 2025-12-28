#include "ui.h"
#include "ui_events.h"
#include <string.h>

void ui_show_AP_qr(const char *ssid, const char *psk)
{
    if (ui_queue != NULL)
    {
        ui_msg_t msg;
        msg.cmd = UI_CMD_SHOW_AP_QR;

        // Copy SSID and PSK into the message structure
        strncpy(msg.payload.wpa_data.ssid, ssid, sizeof(msg.payload.wpa_data.ssid) - 1);
        msg.payload.wpa_data.ssid[sizeof(msg.payload.wpa_data.ssid) - 1] = 0;

        strncpy(msg.payload.wpa_data.psk, psk, sizeof(msg.payload.wpa_data.psk) - 1);
        msg.payload.wpa_data.psk[sizeof(msg.payload.wpa_data.psk) - 1] = 0;

        xQueueSend(ui_queue, &msg, pdMS_TO_TICKS(10));
    }
}

void ui_show_config_qrcode(const char *url)
{
    if (ui_queue != NULL)
    {
        ui_msg_t msg;
        msg.cmd = UI_CMD_SHOW_CONFIG_QR;

        // Copy URL into the message structure
        strncpy(msg.payload.url, url, sizeof(msg.payload.url) - 1);
        msg.payload.url[sizeof(msg.payload.url) - 1] = 0;

        xQueueSend(ui_queue, &msg, pdMS_TO_TICKS(10));
    }
}

void ui_show_clock(void)
{
    if (ui_queue != NULL)
    {
        ui_msg_t msg;
        msg.cmd = UI_CMD_SHOW_CLOCK;

        xQueueSend(ui_queue, &msg, pdMS_TO_TICKS(10));
    }
}

void ui_show_connecting(const char *ssid)
{
    if (ui_queue != NULL)
    {
        ui_msg_t msg;
        msg.cmd = UI_CMD_WIFI_CONNECTING;

        strncpy(msg.payload.wpa_data.ssid, ssid, sizeof(msg.payload.wpa_data.ssid) - 1);
        msg.payload.wpa_data.ssid[sizeof(msg.payload.wpa_data.ssid) - 1] = 0;

        xQueueSend(ui_queue, &msg, pdMS_TO_TICKS(10));
    }
}