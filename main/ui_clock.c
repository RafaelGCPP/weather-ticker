#include <esp_lvgl_port.h>
#include <string.h>
#include <time.h>
#include "ui_internal.h"
#include "openweather_service.h"

bool s_show_clock_mode = false;
int s_last_detected_second = -1;

lv_obj_t *label_hhmmss = NULL;  
lv_obj_t *label_weekday = NULL; 
lv_obj_t *label_date = NULL;    

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

    setup_clock_panel(cont);
    setup_weather_panel(cont);
    setup_precipitation_panel(cont);

    s_last_detected_second = -1;
    s_show_clock_mode = true;
}

void setup_clock_panel(lv_obj_t *parent)
{
    // Left container (Clock)
    lv_obj_t *left_cont = lv_obj_create(parent);
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
            CurrentWeather *current = openweather_get_current_data();
            if (current && right_label)
            {
                lv_label_set_text_fmt(right_label,
                                      "Temp: %.1f C\nFeels: %.1f C\nHumidity: %d%%\n%s",
                                      current->temperature,
                                      current->feelsLike,
                                      current->atmospheric.humidity,
                                      current->weather.main);
            }
            if (precipitation_series && precipitation_chart)
            {
                int32_t minutely_data[NUM_MINUTELY];
                openweather_get_scaled_minutely_precipitation_data(minutely_data);
                lv_chart_set_series_values(precipitation_chart, precipitation_series, minutely_data, NUM_MINUTELY);
                lv_chart_refresh(precipitation_chart);
            }

            openweather_unlock();
        }
    }
}