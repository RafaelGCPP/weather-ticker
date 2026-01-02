#include <esp_lvgl_port.h>
#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "ui_internal.h"
#include "openweather_service.h"
#include "declares.h"

// static const char *TAG = "UI_WEATHER";

lv_obj_t *right_label = NULL;

static const int32_t col_dsc[] = {50, 40, 40, 40, LV_GRID_TEMPLATE_LAST};
static const int32_t row_dsc[] = {10, 20, 20, 20, 20, 20, 10, LV_GRID_TEMPLATE_LAST};

LV_FONT_DECLARE(barlow_condensed_sb42px);
LV_FONT_DECLARE(barlow_condensed_sb24px);

void setup_weather_panel(lv_obj_t *parent)
{
    // Right container (Weather info)
    lv_obj_t *right_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(right_cont, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_opa(right_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_cont, 0, 0);
    lv_obj_set_style_radius(right_cont, 0, 0);
    lv_obj_set_style_pad_all(right_cont, 0, 0);
    lv_obj_set_style_pad_gap(right_cont, 0, 0);
    lv_obj_set_layout(right_cont, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(right_cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(right_cont, row_dsc, 0);
    lv_obj_set_scrollbar_mode(right_cont, LV_SCROLLBAR_MODE_OFF);
    

    lv_obj_t*  icon_img = lv_img_create(right_cont);
    lv_obj_set_grid_cell(icon_img, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_CENTER, 0, 3);
    lv_img_set_src(icon_img, &weather_01d);

    lv_obj_t *temp_label = lv_label_create(right_cont);
    lv_label_set_recolor(temp_label, true);
    lv_obj_set_grid_cell(temp_label, LV_GRID_ALIGN_END, 1, 2, LV_GRID_ALIGN_CENTER, 0, 2);
    lv_label_set_text(temp_label, "#FFFF00 25.3#°C");
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(temp_label, &barlow_condensed_sb24px, 0);

    lv_obj_t *feels_label = lv_label_create(right_cont);
    lv_label_set_recolor(feels_label, true);
    lv_obj_set_grid_cell(feels_label, LV_GRID_ALIGN_END, 1, 2, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_label_set_text(feels_label, "F: #FF0000 35.3#°C");
    lv_obj_set_style_text_color(feels_label, lv_color_hex(0xFFFFFF), 0);

}