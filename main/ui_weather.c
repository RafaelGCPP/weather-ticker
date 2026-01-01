#include <esp_lvgl_port.h>
#include <string.h>
#include "ui_internal.h"
#include "openweather_service.h"

lv_obj_t *right_label = NULL;   

void setup_weather_panel(lv_obj_t *parent)
{
    // Right container (Weather info)
    lv_obj_t *right_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(right_cont, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_opa(right_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_cont, 0, 0);
    lv_obj_set_style_border_side(right_cont, LV_BORDER_SIDE_NONE, 0);
    lv_obj_set_style_border_color(right_cont, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(right_cont, 0, 0);
    lv_obj_set_style_pad_all(right_cont, 0, 0);
    lv_obj_set_layout(right_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(right_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(right_cont, LV_SCROLLBAR_MODE_OFF);

    right_label = lv_label_create(right_cont);
    lv_label_set_text(right_label, "Loading...");
    lv_obj_set_style_text_color(right_label, lv_color_white(), 0);
    lv_obj_set_style_text_font(right_label, &lv_font_montserrat_14, 0);
}