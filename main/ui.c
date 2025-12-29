#include <esp_log.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "ui_events.h"
#include "ui_internal.h"
#include "t_display_s3.h"

// Forward declaration
static void show_connecting_message(const char *ssid);
static void tick_clock(void);
static void setup_clock_screen(void);

// static variables for clock update
static bool s_show_clock_mode = false;
static int s_last_detected_second = -1;

static lv_obj_t *label_status = NULL; //Refactor later...

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



static void setup_clock_screen(void)
{
    // 1. Limpa a tela (remove QR codes antigos)
    lv_obj_clean(lv_screen_active());

    // 2. Cria o label principal
    label_status = lv_label_create(lv_screen_active());

    // Configuração visual básica (depois podemos melhorar fontes)
    lv_obj_set_style_text_color(label_status, lv_color_white(), 0);
    lv_obj_set_style_text_align(label_status, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_status, LV_ALIGN_CENTER, 0, 0);

    // 3. Reseta o estado para forçar atualização imediata
    s_last_detected_second = -1;
    s_show_clock_mode = true;
}

// --- NOVA FUNÇÃO: Atualiza os números (Tick) ---
static void tick_clock(void)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    // Só redesenha se o segundo mudou (evita processamento inútil a cada 100ms)
    if (timeinfo.tm_sec != s_last_detected_second)
    {
        s_last_detected_second = timeinfo.tm_sec;

        if (label_status)
        {
            // Formato: 14:30:05 \n 28/12/2025
            lv_label_set_text_fmt(label_status, "%02d:%02d:%02d\n%02d/%02d/%04d",
                                  timeinfo.tm_hour,
                                  timeinfo.tm_min,
                                  timeinfo.tm_sec,
                                  timeinfo.tm_mday,
                                  timeinfo.tm_mon + 1, // Meses são 0-11
                                  timeinfo.tm_year + 1900);
        }
    }
}

static void show_connecting_message(const char *ssid)
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