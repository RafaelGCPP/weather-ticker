#pragma once

#include "cJSON.h"
#include "typedefs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

float getFloatValue(cJSON *node, const char *key);
int getIntValue(cJSON *node, const char *key);
void getStringValue(cJSON *node, const char *key, char *dest, size_t destSize);

bool parseOpenWeatherData(const char *jsonData, OpenWeatherData *weatherData);
void parseCoordinates(cJSON *key, Coordinates *coord);
void parseAtmosphericForecast(cJSON *node, AtmosphericForecast *atmospheric);
void parseWindForecast(cJSON *node, WindForecast *wind);
void parseWeatherCondition(cJSON *node, WeatherCondition *weather);
void parseTemperatureForecast(cJSON *node, TemperatureForecast *temperature);

void parseCurrentWeather(cJSON *key, CurrentWeather *current);
void parseMinutelyForecast(cJSON *root, MinutelyForecast *minutely);
void parseHourlyForecast(cJSON *root, HourlyForecast *hourly);
void parseDailyForecast(cJSON *root, DailyForecast *daily);

void openweather_service_task(void *pvParameters);
void fetch_and_process_weather_data(Coordinates coord, const char *api_key);

/**
 * @brief Get geocode information (latitude and longitude) for the configured city
 */
Coordinates get_coordinates_from_geocode(const char *api_key);
extern SemaphoreHandle_t s_weather_mutex; 
extern OpenWeatherData *s_weather_data;