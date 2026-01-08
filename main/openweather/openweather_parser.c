#include <cJSON.h>
#include <stdio.h>
#include <string.h>
#include "typedefs.h"
#include "openweather_internal.h"
#include "esp_log.h"

static const char *TAG = "OPENWEATHER_PARSER";

bool parseOpenWeatherData(const char *jsonData, OpenWeatherData *weatherData)
{

    cJSON *root = cJSON_Parse(jsonData);
    if (root == NULL)
    {
        ESP_LOGE(TAG,"Error parsing JSON data.\n");
        return false;
    }

    parseCoordinates(root, &weatherData->coord);
    parseCurrentWeather(root, &weatherData->current);
    parseMinutelyForecast(root, weatherData->minutely);
    parseHourlyForecast(root, weatherData->hourly);
    parseDailyForecast(root, weatherData->daily);

    cJSON_Delete(root);
    return true;
}

float getFloatValue(cJSON *node, const char *key)
{
    cJSON *item = cJSON_GetObjectItem(node, key);
    if (item && cJSON_IsNumber(item))
    {
        return (float)item->valuedouble;
    }
    return 0.0f;
}

int getIntValue(cJSON *node, const char *key)
{
    cJSON *item = cJSON_GetObjectItem(node, key);
    if (item && cJSON_IsNumber(item))
    {
        return item->valueint;
    }
    return 0;
}

void getStringValue(cJSON *node, const char *key, char *dest, size_t destSize)
{
    cJSON *item = cJSON_GetObjectItem(node, key);
    if (item && cJSON_IsString(item))
    {
        strncpy(dest, item->valuestring, destSize - 1);
        dest[destSize - 1] = '\0'; 
    }
    else
    {
        dest[0] = '\0';
    }
}

void parseCoordinates(cJSON *node, Coordinates *coord)
{

    coord->latitude = getFloatValue(node, "lat");
    coord->longitude = getFloatValue(node, "lon");
    coord->timezoneOffset = getIntValue(node, "timezone_offset");
    getStringValue(node, "timezone", coord->timezone, sizeof(coord->timezone));

    return;
}

void parseWindForecast(cJSON *node, WindForecast *wind)
{

    wind->speed = getFloatValue(node, "wind_speed");
    wind->degree = getIntValue(node, "wind_deg");
    wind->gust = getFloatValue(node, "wind_gust");
    return;
}

void parseAtmosphericForecast(cJSON *node, AtmosphericForecast *atmospheric)
{

    atmospheric->pressure = getIntValue(node, "pressure");
    atmospheric->humidity = getIntValue(node, "humidity");
    atmospheric->dewPoint = getFloatValue(node, "dew_point");
    atmospheric->uvi = getFloatValue(node, "uvi");
    atmospheric->clouds = getIntValue(node, "clouds");
    atmospheric->visibility = getIntValue(node, "visibility");

    return;
}

void parseWeatherCondition(cJSON *node, WeatherCondition *weather)
{

    cJSON *weatherArray = cJSON_GetObjectItem(node, "weather");
    if (weatherArray && cJSON_IsArray(weatherArray))
    {
        cJSON *firstWeather = cJSON_GetArrayItem(weatherArray, 0);
        if (firstWeather)
        {
            weather->id = getIntValue(firstWeather, "id");
            getStringValue(firstWeather, "main", weather->main, sizeof(weather->main));
            getStringValue(firstWeather, "description", weather->description, sizeof(weather->description));
            getStringValue(firstWeather, "icon", weather->icon, sizeof(weather->icon));
        }
    }
    else
    {
        weather->id = 0;
        weather->main[0] = '\0';
        weather->description[0] = '\0';
        weather->icon[0] = '\0';
    }

    return;
}

void parseTemperatureForecast(cJSON *node, TemperatureForecast *temperature)
{
    temperature->day = getFloatValue(node, "day");
    temperature->min = getFloatValue(node, "min");
    temperature->max = getFloatValue(node, "max");
    temperature->night = getFloatValue(node, "night");
    temperature->eve = getFloatValue(node, "eve");
    temperature->morn = getFloatValue(node, "morn");
    return;
}

void parseCurrentWeather(cJSON *root, CurrentWeather *current)
{

    cJSON *node = cJSON_GetObjectItem(root, "current");
    if (node)
    {
        current->timestamp = getIntValue(node, "dt");
        current->sunrise = getIntValue(node, "sunrise");
        current->sunset = getIntValue(node, "sunset");
        current->temperature = getFloatValue(node, "temp");
        current->feelsLike = getFloatValue(node, "feels_like");
        parseWindForecast(node, &current->wind);
        parseAtmosphericForecast(node, &current->atmospheric);
        parseWeatherCondition(node, &current->weather);
    }
    else
    {
        current->timestamp = 0;
        current->sunrise = 0;
        current->sunset = 0;
        current->temperature = 0.0f;
        current->feelsLike = 0.0f;
        memset(&current->wind, 0, sizeof(WindForecast));
        memset(&current->atmospheric, 0, sizeof(AtmosphericForecast));
        memset(&current->weather, 0, sizeof(WeatherCondition));
    }

    return;
}

void parseMinutelyForecast(cJSON *root, MinutelyForecast *minutely)
{

    for (int i = 0; i < 60; i++)
    {
        minutely[i].timestamp = 0;
        minutely[i].precipitation = 0.0f;
    }

    cJSON *minutelyArray = cJSON_GetObjectItem(root, "minutely");
    if (minutelyArray && cJSON_IsArray(minutelyArray))
    {
        int count = cJSON_GetArraySize(minutelyArray);
        for (int i = 0; i < count && i < NUM_MINUTELY; i++)
        {
            cJSON *item = cJSON_GetArrayItem(minutelyArray, i);
            minutely[i].timestamp = getIntValue(item, "dt");
            minutely[i].precipitation = getFloatValue(item, "precipitation");
        }
    }

    return;
}

void parseHourlyForecast(cJSON *root, HourlyForecast *hourly)
{
    for (int i = 0; i < NUM_HOURLY; i++)
    {
        hourly[i].timestamp = 0;
        hourly[i].temperature = 0.0f;
        hourly[i].feelsLike = 0.0f;
        memset(&hourly[i].atmospheric, 0, sizeof(AtmosphericForecast));
        memset(&hourly[i].wind, 0, sizeof(WindForecast));
        memset(&hourly[i].weather, 0, sizeof(WeatherCondition));
        hourly[i].pPrecipitation = 0.0f;
    }

    cJSON *hourlyArray = cJSON_GetObjectItem(root, "hourly");
    if (hourlyArray && cJSON_IsArray(hourlyArray))
    {
        int count = cJSON_GetArraySize(hourlyArray);
        for (int i = 0; i < count && i < NUM_HOURLY; i++)
        {
            cJSON *item = cJSON_GetArrayItem(hourlyArray, i);
            hourly[i].timestamp = getIntValue(item, "dt");
            hourly[i].temperature = getFloatValue(item, "temp");
            hourly[i].feelsLike = getFloatValue(item, "feels_like");
            parseAtmosphericForecast(item, &hourly[i].atmospheric);
            parseWindForecast(item, &hourly[i].wind);
            parseWeatherCondition(item, &hourly[i].weather);
            hourly[i].pPrecipitation = getFloatValue(item, "pop");
        }
    }

    return;
}

void parseDailyForecast(cJSON *root, DailyForecast *daily)
{
    for (int i = 0; i < NUM_DAILY; i++)
    {
        daily[i].timestamp = 0;
        memset(&daily[i].temperature, 0, sizeof(TemperatureForecast));
        memset(&daily[i].feelsLike, 0, sizeof(TemperatureForecast));
        memset(&daily[i].atmospheric, 0, sizeof(AtmosphericForecast));
        memset(&daily[i].wind, 0, sizeof(WindForecast));
        memset(&daily[i].weather, 0, sizeof(WeatherCondition));
        daily[i].pPrecipitation = 0.0f;
        daily[i].rainPrecipitation = 0.0f;
        daily[i].snowPrecipitation = 0.0f;
    }

    cJSON *dailyArray = cJSON_GetObjectItem(root, "daily");
    if (dailyArray && cJSON_IsArray(dailyArray))
    {
        int count = cJSON_GetArraySize(dailyArray);
        for (int i = 0; i < count && i < NUM_DAILY; i++)
        {
            cJSON *item = cJSON_GetArrayItem(dailyArray, i);
            daily[i].timestamp = getIntValue(item, "dt");
            cJSON *tempNode = cJSON_GetObjectItem(item, "temp");
            if (tempNode)
            {
                parseTemperatureForecast(tempNode, &daily[i].temperature);
            }
            cJSON *feelsLikeNode = cJSON_GetObjectItem(item, "feels_like");
            if (feelsLikeNode)
            {
                parseTemperatureForecast(feelsLikeNode, &daily[i].feelsLike);
            }
            parseAtmosphericForecast(item, &daily[i].atmospheric);
            parseWindForecast(item, &daily[i].wind);
            parseWeatherCondition(item, &daily[i].weather);
            daily[i].pPrecipitation = getFloatValue(item, "pop");
            daily[i].rainPrecipitation = getFloatValue(item, "rain");
            daily[i].snowPrecipitation = getFloatValue(item, "snow");
        }
    }
    return;
}
