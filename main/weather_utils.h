#pragma once
#include "lvgl.h"

/**
 * Converte o código da OpenWeatherMap (ex: "01d") para o descritor de imagem LVGL.
 * @param code A string do código do ícone (ex: "01d", "10n").
 * @return Ponteiro para a imagem correspondente ou uma imagem padrão se não encontrado.
 */
const lv_image_dsc_t* get_weather_icon(const char* code);


int get_temperature_color(float temperature);

