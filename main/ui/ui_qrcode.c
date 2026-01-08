#include <esp_lvgl_port.h>
#include <string.h>
#include "ui_internal.h"


void show_AP_qrcode(const char *ssid, const char *password)
{
    lv_obj_t *qr_code_obj = NULL;
    lv_obj_t *label_status = NULL;
    show_qrcode(&qr_code_obj, &label_status);
    
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

void show_config_qrcode(const char *url)
{
    lv_obj_t *qr_code_obj = NULL;
    lv_obj_t *label_status = NULL;
    show_qrcode(&qr_code_obj, &label_status);
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
void show_qrcode(lv_obj_t **qr_code_obj_ptr, lv_obj_t **label_status_ptr)
{
    // 1. Clear the screen
    lv_obj_clean(lv_screen_active());

    // --- LEFT SIDE: QR CODE ---

    // The display has 170px height.
    // Using 120px leaves 25px margin on top and bottom.
    lv_coord_t qr_size = 120;

    *qr_code_obj_ptr = lv_qrcode_create(lv_screen_active());
    lv_qrcode_set_size(*qr_code_obj_ptr, qr_size);
    lv_qrcode_set_light_color(*qr_code_obj_ptr, lv_color_white());
    lv_qrcode_set_dark_color(*qr_code_obj_ptr, lv_color_black());
    // Trick: Add a thick white border to create the "Quiet Zone"
    // This REALLY helps the phone read the QR code on a dark background
    lv_obj_set_style_border_color(*qr_code_obj_ptr, lv_color_white(), 0);
    lv_obj_set_style_border_width(*qr_code_obj_ptr, 5, 0);
    // Align to CENTER-LEFT, with 10px margin from screen edge (x=10, y=0)
    lv_obj_align(*qr_code_obj_ptr, LV_ALIGN_LEFT_MID, 10, 0);

    // --- RIGHT SIDE: TEXT ---

    *label_status_ptr = lv_label_create(lv_screen_active());
}