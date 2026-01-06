#include <esp_lvgl_port.h>
#include <string.h>
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "ui_internal.h"
#include "openweather_service.h"
#include "declares.h"
#include "weather_utils.h"

// static const char *TAG = "UI_WEATHER";

lv_obj_t *right_label = NULL;
lv_obj_t *temp_label = NULL;
lv_obj_t *feels_label = NULL;
lv_obj_t *pressure_label = NULL;
lv_obj_t *humidity_label = NULL;
lv_obj_t *dew_label = NULL;
lv_obj_t *uvi_label = NULL;

lv_obj_t *icon_img = NULL;

// CORREÇÃO: 90px + 60px + 60px = 210px (Preenche exatamente o container)
static const int32_t col_dsc[] = {90, 120, LV_GRID_TEMPLATE_LAST};
static const int32_t row_dsc[] = {120, LV_GRID_TEMPLATE_LAST}; // 1 rows: 120px

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

    // First column: Weather Icon and Temperatures
    // Will be a grid with 3 rows and 2 columns
    // Ajustado para caber na coluna de 90px (16 + 16 + 58 = 90)
    static int32_t weather_col_dsc[] = {16, 58, 16, LV_GRID_TEMPLATE_LAST};
    static int32_t weather_row_dsc[] = {60, 32, 28, LV_GRID_TEMPLATE_LAST};

    lv_obj_t *weather_cont = lv_obj_create(right_cont);
    lv_obj_set_style_grid_column_dsc_array(weather_cont, weather_col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(weather_cont, weather_row_dsc, 0);
    lv_obj_set_layout(weather_cont, LV_LAYOUT_GRID);
    lv_obj_set_style_bg_opa(weather_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(weather_cont, 0, 0);
    lv_obj_set_style_pad_all(weather_cont, 0, 0);
    lv_obj_set_style_pad_gap(weather_cont, 0, 0);

    // Tamanho exato da coluna (90px)
    lv_obj_set_size(weather_cont, 90, 120);
    lv_obj_set_grid_cell(weather_cont, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);

    // Weather Icon background
    lv_obj_t *icon_bg = lv_obj_create(weather_cont);
    lv_obj_set_size(icon_bg, 90, 60);                              // Ajustado para largura da coluna
    lv_obj_set_style_bg_color(icon_bg, lv_color_hex(0x00008B), 0); // Dark Blue
    lv_obj_set_style_radius(icon_bg, 0, 0);                        // Quadrado
    lv_obj_set_style_border_width(icon_bg, 0, 0);
    lv_obj_set_grid_cell(icon_bg, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 0, 1);

    icon_img = lv_img_create(weather_cont);
    lv_obj_set_grid_cell(icon_img, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 0, 1); // 60 x 60 px
    lv_img_set_src(icon_img, &weather_01d);

    temp_label = lv_label_create(weather_cont);
    lv_label_set_recolor(temp_label, true);
    // Alinhamento centralizado na largura total (coluna 0 a 3)
    lv_obj_set_grid_cell(temp_label, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_END, 1, 1);
    lv_label_set_text(temp_label, "#FFFF00 25.3#°C");
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(temp_label, &barlow_condensed_sb28px, 0);

    lv_obj_t *feels_icon = lv_img_create(weather_cont);
    lv_img_set_src(feels_icon, &feels_like_icon);
    // Coluna 0 (20px), alinhado à direita
    lv_obj_set_grid_cell(feels_icon, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    feels_label = lv_label_create(weather_cont);
    lv_label_set_recolor(feels_label, true);
    // Coluna 1, alinhado à esquerda
    lv_obj_set_grid_cell(feels_label, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_label_set_text(feels_label, "#FF0000 35.3#°C");
    lv_obj_set_style_text_color(feels_label, lv_color_hex(0xFFFFFF), 0);

    // Second column: atmospheric data (Pressure, Humidity, etc.)
    static int32_t atm_col_dsc[] = {20, 16, 64, 20, LV_GRID_TEMPLATE_LAST};
    static int32_t atm_row_dsc[] = {20, 20, 20, 20, 20, 20, LV_GRID_TEMPLATE_LAST};

    lv_obj_t *atm_cont = lv_obj_create(right_cont);
    lv_obj_set_style_grid_column_dsc_array(atm_cont, atm_col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(atm_cont, atm_row_dsc, 0);
    lv_obj_set_layout(atm_cont, LV_LAYOUT_GRID);
    lv_obj_set_style_bg_opa(atm_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(atm_cont, 0, 0);
    lv_obj_set_style_pad_all(atm_cont, 0, 0);
    lv_obj_set_style_pad_gap(atm_cont, 0, 0);
    // Tamanho exato da coluna (60px)
    lv_obj_set_size(atm_cont, 120, 120);
    lv_obj_set_grid_cell(atm_cont, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);


    lv_obj_t *pr_icon = lv_img_create(atm_cont);
    lv_img_set_src(pr_icon, &pressure_icon); // change to pressure icon
    // Coluna 0 (20px), alinhado à direita
    lv_obj_set_grid_cell(pr_icon, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    pressure_label = lv_label_create(atm_cont);
    lv_label_set_text_fmt(pressure_label, "%4dhPa", 0);
    lv_obj_set_grid_cell(pressure_label, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_color(pressure_label, lv_color_white(), 0);

    lv_obj_t *hm_icon = lv_img_create(atm_cont);
    lv_img_set_src(hm_icon, &humidity_icon); // change to humidity icon
    // Coluna 0 (20px), alinhado à direita
    lv_obj_set_grid_cell(hm_icon, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);

    humidity_label = lv_label_create(atm_cont);
    lv_label_set_text_fmt(humidity_label, "%4dhPa", 0);
    lv_obj_set_grid_cell(humidity_label, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_style_text_color(humidity_label, lv_color_white(), 0);

    
    lv_obj_t *dw_icon = lv_img_create(atm_cont);
    lv_img_set_src(dw_icon, &humidity_icon); // change to humidity icon
    // Coluna 0 (20px), alinhado à direita
    lv_obj_set_grid_cell(dw_icon, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    dew_label = lv_label_create(atm_cont);
    lv_label_set_text_fmt(dew_label, "%.1f°C", 0.0);
    lv_obj_set_grid_cell(dew_label, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_style_text_color(dew_label, lv_color_white(), 0);

    lv_obj_t *uv_icon = lv_img_create(atm_cont);
    lv_img_set_src(uv_icon, &uvi_icon); // change to humidity icon
    // Coluna 0 (20px), alinhado à direita
    lv_obj_set_grid_cell(uv_icon, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    uvi_label = lv_label_create(atm_cont);
    lv_label_set_text_fmt(uvi_label, "%.1f°C", 0.0);
    lv_obj_set_grid_cell(uvi_label, LV_GRID_ALIGN_END, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_style_text_color(uvi_label, lv_color_white(), 0);
}

void update_weather_display() // called from ui_clock.c tick_clock() every second.
{
    if (openweather_lock(50))
    {
        CurrentWeather *current = openweather_get_current_data();
        if (current)
        {
            if (temp_label)
            {
                lv_label_set_text_fmt(temp_label, "#%06x %.1f#°C", get_temperature_color(current->temperature), current->temperature);
            }
            if (feels_label)
            {
                lv_label_set_text_fmt(feels_label, "#%06x %.1f#°C", get_temperature_color(current->feelsLike), current->feelsLike);
            }
            if (pressure_label)
            {
                lv_label_set_text_fmt(pressure_label, "%4dhPa", current->atmospheric.pressure);
            }
            if (humidity_label)
            {
                lv_label_set_text_fmt(humidity_label, "%3d%%", current->atmospheric.humidity);
            }
            if (dew_label)
            {
                lv_label_set_text_fmt(dew_label, "%.1f°C", current->atmospheric.dewPoint);
            }
            if (uvi_label)
            {
                lv_label_set_text_fmt(uvi_label, "%.1f", current->atmospheric.uvi);
            }
        }
        if (precipitation_series && precipitation_chart)
        {
            int32_t minutely_data[NUM_MINUTELY];
            openweather_get_scaled_minutely_precipitation_data(minutely_data);
            lv_chart_set_series_values(precipitation_chart, precipitation_series, minutely_data, NUM_MINUTELY);
            lv_chart_refresh(precipitation_chart);
        }
        if (icon_img && current)
        {
            const lv_image_dsc_t *icon = get_weather_icon(current->weather.icon);
            lv_img_set_src(icon_img, icon);
        }
        openweather_unlock();
    }
}