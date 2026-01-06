#include <esp_lvgl_port.h>
#include <string.h>
#include "ui_internal.h"
#include "openweather_service.h"

lv_obj_t *precipitation_chart = NULL;
lv_chart_series_t *precipitation_series = NULL;


static const int HEAVY_RAIN_THRESHOLD = 60;  // mm/h
static const int MEDIUM_RAIN_THRESHOLD = 30; // mm/h
static const int LIGHT_RAIN_THRESHOLD = 10;  // mm/h

static void chart_draw_event_cb(lv_event_t *e);

void setup_precipitation_panel(lv_obj_t *parent)
{
    // Bottom container (Graph/precipitation)
    lv_obj_t *bottom_cont = lv_obj_create(parent);
    lv_obj_set_grid_cell(bottom_cont, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_style_bg_opa(bottom_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(bottom_cont, 1, 0);
    lv_obj_set_style_border_side(bottom_cont, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_border_color(bottom_cont, lv_color_hex(0x404040), 0);
    lv_obj_set_style_radius(bottom_cont, 0, 0);
    lv_obj_set_style_pad_all(bottom_cont, 0, 0);
    lv_obj_set_scrollbar_mode(bottom_cont, LV_SCROLLBAR_MODE_OFF);

    // Precipitation bar chart (60 colunas)
    precipitation_chart = lv_chart_create(bottom_cont);
    lv_obj_set_size(precipitation_chart, 300, 40);
    lv_obj_center(precipitation_chart);
    lv_obj_set_style_bg_opa(precipitation_chart, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(precipitation_chart, 0, 0);
    lv_obj_set_style_pad_all(precipitation_chart, 0, 0);
    lv_obj_set_style_pad_right(precipitation_chart, 1, 0);
    lv_obj_set_style_pad_bottom(precipitation_chart, 1, 0);

    lv_chart_set_type(precipitation_chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(precipitation_chart, NUM_MINUTELY);
    lv_chart_set_range(precipitation_chart, LV_CHART_AXIS_PRIMARY_Y, 0, HEAVY_RAIN_THRESHOLD);
    lv_chart_set_div_line_count(precipitation_chart, 0, 0);

    // Configuração crítica para barras espaçadas
    lv_obj_set_style_pad_column(precipitation_chart, 1, LV_PART_MAIN);
    lv_obj_set_style_line_color(precipitation_chart, lv_color_hex(0x606060), LV_PART_MAIN);

    lv_obj_set_style_line_dash_width(precipitation_chart, 2, LV_PART_MAIN);
    lv_obj_set_style_line_dash_gap(precipitation_chart, 2, LV_PART_MAIN);
    lv_chart_set_div_line_count(precipitation_chart, 4, 7);

    // Adiciona uma série
    precipitation_series = lv_chart_add_series(precipitation_chart, lv_color_hex(0x00BFFF), LV_CHART_AXIS_PRIMARY_Y);

    lv_obj_add_event_cb(precipitation_chart, chart_draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(precipitation_chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    lv_chart_set_all_values(precipitation_chart, precipitation_series, 0);

    lv_chart_refresh(precipitation_chart);
}

static void chart_draw_event_cb(lv_event_t *e)
{
    lv_draw_task_t *draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t *base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);

    if (base_dsc->part != LV_PART_ITEMS)
    {
        return;
    }

    lv_draw_fill_dsc_t *fill_dsc = lv_draw_task_get_fill_dsc(draw_task);
    if (fill_dsc)
    {
        lv_obj_t *chart = lv_event_get_target_obj(e);
        int32_t *y_array = lv_chart_get_series_y_array(chart, lv_chart_get_series_next(chart, NULL));
        int32_t v = y_array[base_dsc->id2];

        if (v <= LIGHT_RAIN_THRESHOLD)
        {
            fill_dsc->color = lv_color_hex(0x00BFFF); // Azul claro para baixa precipitação
        }
        else if (v <= MEDIUM_RAIN_THRESHOLD)
        {
            fill_dsc->color = lv_color_hex(0x1E90FF); // Azul médio para precipitação moderada
        }
        else if (v <= HEAVY_RAIN_THRESHOLD)
        {
            fill_dsc->color = lv_color_hex(0x0000FF); // Azul escuro para alta precipitação
        }
        else
        {
            fill_dsc->color = lv_color_hex(0xFF0000); // Vermelho intenso para precipitação muito alta
        }
    }
}
