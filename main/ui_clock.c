#include <esp_lvgl_port.h>
#include <string.h>
#include <time.h>
#include "ui_internal.h"

bool s_show_clock_mode = false;
int s_last_detected_second = -1;

static lv_obj_t *label_hhmmss = NULL;  // Refactor later...
static lv_obj_t *label_weekday = NULL; // Refactor later...
static lv_obj_t *label_date = NULL;    // Refactor later...

static const int32_t col_dsc[] = {150, 170, LV_GRID_TEMPLATE_LAST}; /* 2 columns with 150- and 170-px width */
static const int32_t row_dsc[] = {170, LV_GRID_TEMPLATE_LAST};      /* `1 170-px tall rows */

LV_FONT_DECLARE(barlow_condensed_sb42px); // Declare custom font
LV_FONT_DECLARE(montserrat_sb14px);          // Declare custom font

void setup_clock_screen(void)
{
    lv_obj_clean(lv_screen_active());

    // Main grid container
    // Two columns, one row. The left panel will be a clock
    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 320, 170);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x101010), LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_margin_all(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

    // left container with flex layout
    lv_obj_t *left_cont = lv_obj_create(cont);
    lv_obj_set_grid_cell(left_cont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_opa(left_cont, LV_OPA_TRANSP, 0); // Transparent background
    lv_obj_set_style_border_width(left_cont, 0, 0);
    lv_obj_set_style_pad_all(left_cont, 0, 0);
    lv_obj_set_layout(left_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(left_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    label_weekday = lv_label_create(left_cont);
    lv_obj_set_style_text_color(label_weekday, lv_color_hex(0xFFA500), 0); // Orange
    lv_obj_set_style_text_font(label_weekday, &montserrat_sb14px, 0);

    label_hhmmss = lv_label_create(left_cont);
    lv_obj_set_style_text_color(label_hhmmss, lv_color_white(), 0);
    lv_obj_set_style_text_font(label_hhmmss, &barlow_condensed_sb42px, 0);

    label_date = lv_label_create(left_cont);
    lv_obj_set_style_text_color(label_date, lv_color_hex(0xD3D3D3), 0); // Light Grey
    lv_obj_set_style_text_font(label_date, &lv_font_montserrat_14, 0);

    // right panel with just a label as placeholder
    lv_obj_t *right_label = lv_label_create(cont);
    lv_obj_set_grid_cell(right_label, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_text(right_label, "Placeholder\nfor weather\nor other info");
    lv_obj_set_style_text_color(right_label, lv_color_white(), 0);

    // 3. Reseta o estado para forçar atualização imediata
    s_last_detected_second = -1;
    s_show_clock_mode = true;
}

// --- NOVA FUNÇÃO: Atualiza os números (Tick) ---
void tick_clock(void)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

    // Só redesenha se o segundo mudou (evita processamento inútil a cada 100ms)
    if (timeinfo.tm_sec != s_last_detected_second)
    {
        s_last_detected_second = timeinfo.tm_sec;

        if (label_weekday)
        {
            char weekday[16];
            strftime(weekday, sizeof(weekday), "%A", &timeinfo);
            lv_label_set_text(label_weekday, weekday);
        }

        if (label_hhmmss)
        {
            // Formato: 14:30:05 \n 28/12/2025
            lv_label_set_text_fmt(label_hhmmss, "%02d:%02d:%02d",
                                  timeinfo.tm_hour,
                                  timeinfo.tm_min,
                                  timeinfo.tm_sec);
        }

        if (label_date)
        {
            lv_label_set_text_fmt(label_date, "%02d/%02d/%04d",
                                  timeinfo.tm_mday,
                                  timeinfo.tm_mon + 1, // Meses são 0-11
                                  timeinfo.tm_year + 1900);
        }
    }
}
