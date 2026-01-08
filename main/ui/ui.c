#include <esp_log.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "ui_events.h"
#include "ui_internal.h"
#include "t_display_s3.h"


// Constants
static const char *TAG = "UI";

// UI Global Variables
QueueHandle_t ui_queue = NULL;

// Initialize UI Queue
void ui_init_queue(void)
{
    ui_queue = xQueueCreate(5, sizeof(ui_msg_t));
    if (ui_queue == NULL)
    {
        ESP_LOGE(TAG, "Failed to create UI queue");
    }
}

void update_ui(void)
{
    ui_msg_t msg;

    // Check if there's a message in the queue (non-blocking)
    if (ui_queue != NULL && xQueueReceive(ui_queue, &msg, 0) == pdTRUE)
    {
        ESP_LOGI(TAG, "Command received: %d", msg.cmd);

        switch (msg.cmd)
        {
        case UI_CMD_SHOW_AP_QR:
            s_show_clock_mode = false;
            show_AP_qrcode(msg.payload.wpa_data.ssid, msg.payload.wpa_data.psk);
            break;

        case UI_CMD_SHOW_CONFIG_QR:
            s_show_clock_mode = false;
            show_config_qrcode(msg.payload.url);
            break;

        case UI_CMD_SHOW_CLOCK:
            setup_clock_screen();
            break;

        case UI_CMD_WIFI_CONNECTING:
            s_show_clock_mode = false; 
            show_connecting_message(msg.payload.wpa_data.ssid);
            break;

        default:
            break;
        }
    }
    if (s_show_clock_mode)
    {
        tick_clock();
    }
}

void ui_init(void)
{
    /*Change the active screen's background color*/
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    /*Create a white label, set its text and align it to the center*/
    lv_obj_t *label_status = lv_label_create(lv_screen_active());
    lv_label_set_text(label_status, "Booting...");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label_status, LV_ALIGN_CENTER, 0, 0);
}

void show_connecting_message(const char *ssid)
{
    // 1. Clear the screen
    lv_obj_clean(lv_screen_active());

    // 2. Create the label
    lv_obj_t *label_status = lv_label_create(lv_screen_active());

    // 3. Set the text
    lv_label_set_text_fmt(label_status, "Connecting to\n%s...", ssid);

    // 4. Style the text
    lv_obj_set_style_text_color(label_status, lv_color_white(), 0); // White text
    lv_obj_align(label_status, LV_ALIGN_CENTER, 0, 0);
}



