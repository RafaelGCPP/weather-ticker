#include "ui.h"
#include "ui_events.h"
#include <string.h>

void ui_show_AP_qr(const char *ssid, const char *psk)
{
    if (ui_queue != NULL)
    {
        ui_msg_t msg;
        msg.cmd = UI_CMD_SHOW_AP_QR;

        // Copia SSID e Senha seguros
        strncpy(msg.ssid, ssid, sizeof(msg.ssid) - 1);
        msg.ssid[sizeof(msg.ssid) - 1] = 0;

        strncpy(msg.password, psk, sizeof(msg.password) - 1);
        msg.password[sizeof(msg.password) - 1] = 0;

        xQueueSend(ui_queue, &msg, pdMS_TO_TICKS(10));
    }
}