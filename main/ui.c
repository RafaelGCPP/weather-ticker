#include <esp_log.h>
#include <string.h>
#include <time.h>
#include "ui.h"
#include "ui_events.h"
#include "t_display_s3.h"

// Forward declaration
static void show_AP_qrcode(const char *ssid, const char *password);
static void show_config_qrcode(const char *url);
static void show_qrcode();
static void show_connecting_message(const char *ssid);
static void tick_clock(void);
static void setup_clock_screen(void);

// static variables for clock update
static bool s_show_clock_mode = false;
static int s_last_detected_second = -1;

// Constants
static const char *TAG = "UI";

// UI Global Variables
QueueHandle_t ui_queue = NULL;
lv_obj_t *label_status = NULL;
lv_obj_t *qr_code_obj = NULL;

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
    label_status = lv_label_create(lv_screen_active());
    lv_label_set_text(label_status, "Booting...");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label_status, LV_ALIGN_CENTER, 0, 0);
}

static void show_AP_qrcode(const char *ssid, const char *password)
{
    show_qrcode();
    // --- PAYLOAD GENERATION ---
    char payload[150];
    if (password && strlen(password) > 0)
    {
        snprintf(payload, sizeof(payload), "WIFI:S:%s;T:WPA;P:%s;;", ssid, password);
    }
    else
    {
        snprintf(payload, sizeof(payload), "WIFI:S:%s;T:nopass;;", ssid);
    }

    lv_qrcode_update(qr_code_obj, payload, strlen(payload));

    // Set text with line breaks (\n)
    lv_label_set_text_fmt(label_status, "Scan to config,\nor enter:\n\nSSID: %s\nPSK: %s", ssid, password);
    // Text Styling
    lv_obj_set_style_text_color(label_status, lv_color_white(), 0); // White text

    // Set max width so text doesn't overflow if SSID is long
    // 320 (total width) - 10 (left margin) - 120 (QR) - 15 (spacing) = ~175px available
    lv_obj_set_width(label_status, 169);
    lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP); // Wrap line if too long

    // The Alignment Trick:
    // Align text "OUTSIDE, TO THE RIGHT" (OUT_RIGHT) of the QR Code object
    // With 15px spacing between them (x=15, y=0)
    lv_obj_align_to(label_status, qr_code_obj, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
}

static void show_config_qrcode(const char *url)
{
    show_qrcode();
    // --- PAYLOAD GENERATION ---
    char payload[150];

    snprintf(payload, sizeof(payload), "%s", url);

    lv_qrcode_update(qr_code_obj, payload, strlen(payload));

    // Set text with line breaks (\n)
    lv_label_set_text_fmt(label_status, "Scan to config,\nor browse to:\n%s", url);
    // Text Styling
    lv_obj_set_style_text_color(label_status, lv_color_white(), 0); // White text

    // Set max width so text doesn't overflow if SSID is long
    // 320 (total width) - 10 (left margin) - 120 (QR) - 15 (spacing) = ~175px available
    lv_obj_set_width(label_status, 169);
    lv_label_set_long_mode(label_status, LV_LABEL_LONG_WRAP); // Wrap line if too long

    // The Alignment Trick:
    // Align text "OUTSIDE, TO THE RIGHT" (OUT_RIGHT) of the QR Code object
    // With 15px spacing between them (x=15, y=0)
    lv_obj_align_to(label_status, qr_code_obj, LV_ALIGN_OUT_RIGHT_MID, 15, 0);
}

// Helper function to create/update QR Code
static void show_qrcode()
{
    // 1. Clear the screen
    lv_obj_clean(lv_screen_active());

    // --- LEFT SIDE: QR CODE ---

    // The display has 170px height.
    // Using 120px leaves 25px margin on top and bottom.
    lv_coord_t qr_size = 120;

    qr_code_obj = lv_qrcode_create(lv_screen_active());
    lv_qrcode_set_size(qr_code_obj, qr_size);
    lv_qrcode_set_light_color(qr_code_obj, lv_color_white());
    lv_qrcode_set_dark_color(qr_code_obj, lv_color_black());
    // Trick: Add a thick white border to create the "Quiet Zone"
    // This REALLY helps the phone read the QR code on a dark background
    lv_obj_set_style_border_color(qr_code_obj, lv_color_white(), 0);
    lv_obj_set_style_border_width(qr_code_obj, 5, 0);

    // Align to CENTER-LEFT, with 10px margin from screen edge (x=10, y=0)
    lv_obj_align(qr_code_obj, LV_ALIGN_LEFT_MID, 10, 0);

    // --- RIGHT SIDE: TEXT ---

    label_status = lv_label_create(lv_screen_active());
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
    label_status = lv_label_create(lv_screen_active());

    // 3. Set the text
    lv_label_set_text_fmt(label_status, "Connecting to\n%s...", ssid);

    // 4. Style the text
    lv_obj_set_style_text_color(label_status, lv_color_white(), 0); // White text
    lv_obj_align(label_status, LV_ALIGN_CENTER, 0, 0);
}