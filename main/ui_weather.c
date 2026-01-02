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
lv_obj_t *icon_img = NULL;

static const int32_t col_dsc[] =
    {
        10, 10, 10, 10, 10,
        10, 10, 10, 10, 10,
        10, 10, 10, 10, 10,
        10, 10, 10, 10, 10,
        LV_GRID_TEMPLATE_LAST}; // 20 columns of 10 px
static const int32_t row_dsc[] =
    {
        10, 10, 10, 10, 10,
        10, 10, 10, 10, 10,
        10, 10, 10, 10, 10,
        LV_GRID_TEMPLATE_LAST}; // 12 rows of 20 px

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

    
    for (int i = 0; i < 20; i++) {     // Colunas
        for (int j = 0; j < 12; j++) { // Linhas
            lv_obj_t *cell = lv_obj_create(right_cont);
            lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_STRETCH, j, 1);
            lv_obj_set_style_bg_opa(cell, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_width(cell, 1, 0);
            lv_obj_set_style_border_color(cell, lv_color_hex(0x303030), 0); // Cinza escuro
            lv_obj_set_style_radius(cell, 0, 0);
            lv_obj_move_background(cell); // Manda para trás dos itens reais
        }
    }
    

    lv_obj_t *icon_bg = lv_obj_create(right_cont);
    lv_obj_set_size(icon_bg, 80, 60);
    lv_obj_set_style_bg_color(icon_bg, lv_color_hex(0x00008B), 0); // Dark Blue
    lv_obj_set_style_radius(icon_bg, 0, 0); // Quadrado
    lv_obj_set_style_border_width(icon_bg, 0, 0);
    lv_obj_set_grid_cell(icon_bg, LV_GRID_ALIGN_CENTER, 0, 8, LV_GRID_ALIGN_CENTER, 0, 6);

    icon_img = lv_img_create(right_cont);
    lv_obj_set_grid_cell(icon_img, LV_GRID_ALIGN_CENTER, 0, 8, LV_GRID_ALIGN_CENTER, 0, 6); // 60 x 60 px
    lv_img_set_src(icon_img, &weather_01d);

    temp_label = lv_label_create(right_cont);
    lv_label_set_recolor(temp_label, true);
    lv_obj_set_grid_cell(temp_label, LV_GRID_ALIGN_CENTER, 0, 8, LV_GRID_ALIGN_CENTER, 6, 3);
    lv_label_set_text(temp_label, "#FFFF00 25.3#°C");
    lv_obj_set_style_text_color(temp_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(temp_label, &barlow_condensed_sb28px, 0);



    // Feels like container
    lv_obj_t *feels_cont = lv_obj_create(right_cont);
    lv_obj_set_style_bg_opa(feels_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(feels_cont, 0, 0);
    lv_obj_set_style_pad_all(feels_cont, 0, 0);
    
    // Define tamanho 100% da largura para achar o centro corretamente
    lv_obj_set_size(feels_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(feels_cont, LV_GRID_ALIGN_CENTER, 0, 8, LV_GRID_ALIGN_CENTER, 9, 3);

    // Configura Grid: 2 colunas ajustadas ao conteúdo (CONTENT)
    static int32_t clock_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    static int32_t clock_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    
    lv_obj_set_layout(feels_cont, LV_LAYOUT_GRID);
    lv_obj_set_style_grid_column_dsc_array(feels_cont, clock_col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(feels_cont, clock_row_dsc, 0);
    
    // Centraliza o grupo de colunas dentro do container
    lv_obj_set_style_grid_column_align(feels_cont, LV_GRID_ALIGN_CENTER, 0);
    // Garante que não haja espaço extra entre as colunas
    lv_obj_set_style_pad_column(feels_cont, 0, 0); 

    lv_obj_t *thermometer_icon = lv_img_create(feels_cont);
    lv_obj_set_grid_cell(thermometer_icon, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_img_set_src(thermometer_icon, &feels_like_icon);

    feels_label = lv_label_create(feels_cont);
    lv_label_set_recolor(feels_label, true);
    lv_obj_set_grid_cell(feels_label, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_text(feels_label, "#FF0000 35.3#°C");
    lv_obj_set_style_text_color(feels_label, lv_color_hex(0xFFFFFF), 0);
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