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
        strncpy(msg.wpa_data.ssid, ssid, sizeof(msg.wpa_data.ssid) - 1);
        msg.wpa_data.ssid[sizeof(msg.wpa_data.ssid) - 1] = 0;

        strncpy(msg.wpa_data.password, psk, sizeof(msg.wpa_data.password) - 1);
        msg.wpa_data.password[sizeof(msg.wpa_data.password) - 1] = 0;

        xQueueSend(ui_queue, &msg, pdMS_TO_TICKS(10));
    }
}