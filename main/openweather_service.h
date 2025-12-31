#pragma once

#include "typedefs.h"
#include "esp_err.h"



void openweather_service_init();
bool openweather_lock(int timeout);
void openweather_unlock();
OpenWeatherData* openweather_get_data();