#include "weather_utils.h"
#include <string.h>
#include "declares.h"


int get_temperature_color(float temperature)
{
    if (temperature <= 0.0f) {
        return 0x00BFFF; // Blue
    } else if (temperature <= 15.0f) {
        return 0x7FFFD4; // Aquamarine
    } else if (temperature <= 25.0f) {
        return 0xFFFF00; // Yellow
    } else if (temperature <= 35.0f) {
        return 0xFFA500; // Orange
    } else {
        return 0xFF0000; // Red
    }
}

// Definimos uma estrutura para nossa tabela de mapeamento
typedef struct {
    const char *code;            // O código da API (ex: "01d")
    const lv_image_dsc_t *img;   // O ponteiro para a imagem gerada
} weather_map_t;

// A Tabela de Consulta
// Mapeia diretamente o texto da API para a variável gerada
static const weather_map_t icon_map[] = {
    // Céu Limpo
    {"01d", &weather_01d},
    {"01n", &weather_01n},
    
    // Poucas Nuvens
    {"02d", &weather_02d},
    {"02n", &weather_02n},
    
    // Nuvens Dispersas
    {"03d", &weather_03},
    {"03n", &weather_03},
    
    // Nublado (Broken Clouds)
    {"04d", &weather_04},
    {"04n", &weather_04},
    
    // Chuva de Banho (Shower Rain)
    {"09d", &weather_09},
    {"09n", &weather_09},
    
    // Chuva (Rain)
    {"10d", &weather_10d},
    {"10n", &weather_10n},
    
    // Tempestade
    {"11d", &weather_11d},
    {"11n", &weather_11n},
    
    // Neve
    {"13d", &weather_13},
    {"13n", &weather_13},
    
    // Névoa (Mist)
    {"50d", &weather_50},
    {"50n", &weather_50},
    
    // Fim da lista (sentinela)
    {NULL, NULL} 
};

// Imagem de fallback caso venha um código estranho ou erro
// (Certifique-se de ter um unknown.svg na pasta e gerado pelo script)
// Se não tiver, aponte para &weather_01d provisoriamente
#define FALLBACK_ICON &weather_na

const lv_image_dsc_t* get_weather_icon(const char* code) {
    if (code == NULL) return FALLBACK_ICON;

    // Percorre a tabela procurando o código
    for (int i = 0; icon_map[i].code != NULL; i++) {
        if (strcmp(code, icon_map[i].code) == 0) {
            return icon_map[i].img; // Encontrou!
        }
    }

    // Se varreu tudo e não achou
    return FALLBACK_ICON;
}