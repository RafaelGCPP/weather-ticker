#pragma once

#include "typedefs.h"
#include "esp_err.h"



void openweather_service_init();
bool openweather_lock(int timeout);
void openweather_unlock();
CurrentWeather* openweather_get_current_data();
void openweather_get_scaled_minutely_precipitation_data(int32_t *data_out);