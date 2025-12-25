#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "t_display_s3.h"
#include "display.h"
#include "ui.h"
#include "ui_events.h"


static lv_disp_t *disp_handle = NULL;

static void ui_update_task(void *pvParam);

void initialize_display(void)
{
    ui_init_queue();

    lcd_init(&disp_handle, true);

    xTaskCreatePinnedToCore(ui_update_task, "update_ui", 4096 * 2, NULL, 0, NULL, 1);

}


static void ui_update_task(void *pvParam) {
    // setup the test ui
    lvgl_port_lock(0);
    ui_init();
    lvgl_port_unlock();

    while (1) {
        // update the ui every 100 milliseconds
        vTaskDelay(pdMS_TO_TICKS(100));
        if (lvgl_port_lock(0)) {
            // update ui under lvgl semaphore lock
            update_ui();
            lvgl_port_unlock();
        }
    }

    // a freeRTOS task should never return ^^^
}



