#include <esp_lvgl_port.h>
#include <string.h>
#include <time.h>
#include "ui_internal.h"
#include "openweather_service.h"

bool s_show_clock_mode = false;
int s_last_detected_second = -1;

static lv_obj_t *label_hhmmss = NULL;  // Refactor later...
static lv_obj_t *label_weekday = NULL; // Refactor later...
static lv_obj_t *label_date = NULL;    // Refactor later...
static lv_obj_t *right_label = NULL;   // Placeholder for right panel

static const int32_t col_dsc[] = {150, 170, LV_GRID_TEMPLATE_LAST}; /* 2 columns with 150- and 170-px width */
static const int32_t row_dsc[] = {120, 50, LV_GRID_TEMPLATE_LAST};  /* 2 rows: 120px + 50px */

LV_FONT_DECLARE(barlow_condensed_sb42px); // Declare custom font
LV_FONT_DECLARE(montserrat_sb14px);       // Declare custom font

void setup_clock_screen(void)
{
    lv_obj_clean(lv_screen_active());

    // Main grid container
    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_size(cont, 320, 170);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x101010), LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);          
    lv_obj_set_style_pad_gap(cont, 0, 0);         
    lv_obj_center(cont);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);  

    // Left container (Clock)
    lv_obj_t *left_cont = lv_obj_create(cont);
    lv_obj_set_grid_cell(left_cont, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_style_bg_opa(left_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(left_cont, 1, 0);
    lv_obj_set_style_border_side(left_cont, LV_BORDER_SIDE_RIGHT, 0);
    lv_obj_set_style_border_color(left_cont, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(left_cont, 0, 0);
    lv_obj_set_style_pad_all(left_cont, 0, 0);      
    lv_obj_set_layout(left_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(left_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(left_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(left_cont, LV_SCROLLBAR_MODE_OFF);

    label_weekday = lv_label_create(left_cont);
    lv_obj_set_style_text_color(label_weekday, lv_color_hex(0xFFA500), 0);
    lv_obj_set_style_text_font(label_weekday, &montserrat_sb14px, 0);

    label_hhmmss = lv_label_create(left_cont);
    lv_obj_set_style_text_color(label_hhmmss, lv_color_white(), 0);
    lv_obj_set_style_text_font(label_hhmmss, &barlow_condensed_sb42px, 0);

    label_date = lv_label_create(left_cont);
    lv_obj_set_style_text_color(label_date, lv_color_hex(0xD3D3D3), 0);
    lv_obj_set_style_text_font(label_date, &lv_font_montserrat_14, 0);

    // Right container (Weather info)
    lv_obj_t *right_cont = lv_obj_create(cont);
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

    // Bottom container (Graph/precipitation)
    lv_obj_t *bottom_cont = lv_obj_create(cont);
    lv_obj_set_grid_cell(bottom_cont, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_style_bg_opa(bottom_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bottom_cont, 1, 0);
    lv_obj_set_style_border_side(bottom_cont, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_color(bottom_cont, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(bottom_cont, 0, 0);
    lv_obj_set_style_pad_all(bottom_cont, 0, 0);
    lv_obj_set_layout(bottom_cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(bottom_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(bottom_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_scrollbar_mode(bottom_cont, LV_SCROLLBAR_MODE_OFF);

    s_last_detected_second = -1;
    s_show_clock_mode = true;
}

void tick_clock(void)
{
    time_t now;
    struct tm timeinfo;

    time(&now);
    localtime_r(&now, &timeinfo);

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
            lv_label_set_text_fmt(label_hhmmss, "%02d:%02d:%02d",
                                  timeinfo.tm_hour,
                                  timeinfo.tm_min,
                                  timeinfo.tm_sec);
        }

        if (label_date)
        {
            lv_label_set_text_fmt(label_date, "%02d/%02d/%04d",
                                  timeinfo.tm_mday,
                                  timeinfo.tm_mon + 1,
                                  timeinfo.tm_year + 1900);
        }

        if (openweather_lock(50))
        {
            OpenWeatherData *weather_data = openweather_get_data();
            if (weather_data != NULL)
            {
                lv_label_set_text_fmt(right_label,
                                      "Temp: %.1f C\nHumidity: %d%%\n%s",
                                      weather_data->current.temperature,
                                      weather_data->current.atmospheric.humidity,
                                      weather_data->current.weather.main);
            }
            openweather_unlock();
        }
    }
}