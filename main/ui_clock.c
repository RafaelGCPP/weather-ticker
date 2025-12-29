#include <esp_lvgl_port.h>
#include <string.h>
#include <time.h>
#include "ui_internal.h"


bool s_show_clock_mode = false;
int s_last_detected_second = -1;



static lv_obj_t *label_status = NULL; //Refactor later...

void setup_clock_screen(void)
{
    // 1. Limpa a tela (remove QR codes antigos)
    lv_obj_clean(lv_screen_active());

    // 2. Cria o label principal
    label_status = lv_label_create(lv_screen_active());

    // Configuração visual básica (depois podemos melhorar fontes)
    lv_obj_set_style_text_color(label_status, lv_color_white(), 0);
    lv_obj_set_style_text_align(label_status, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label_status, LV_ALIGN_CENTER, 0, 0);

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

        if (label_status)
        {
            // Formato: 14:30:05 \n 28/12/2025
            lv_label_set_text_fmt(label_status, "%02d:%02d:%02d\n%02d/%02d/%04d",
                                  timeinfo.tm_hour,
                                  timeinfo.tm_min,
                                  timeinfo.tm_sec,
                                  timeinfo.tm_mday,
                                  timeinfo.tm_mon + 1, // Meses são 0-11
                                  timeinfo.tm_year + 1900);
        }
    }
}
