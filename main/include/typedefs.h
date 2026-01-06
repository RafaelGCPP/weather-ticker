#pragma once

#define MAX_SSID_LEN 32
#define MAX_PSK_LEN 64
#define NUM_MINUTELY 60
#define NUM_HOURLY 48
#define NUM_DAILY 7

typedef struct {
    float latitude;
    float longitude;
    char timezone[32];
    int timezoneOffset;
} Coordinates;

typedef struct {
    int id;
    char main[32];
    char description[64];
    char icon[8];
} WeatherCondition;

typedef struct {
    float speed;
    int degree;
    float gust;
} WindForecast;

typedef struct {
    int pressure;
    int humidity;
    float dewPoint;
    float uvi;
    int clouds;
    int visibility;
} AtmosphericForecast;

typedef struct {
    int timestamp;
    int sunrise;
    int sunset;
    float temperature;
    float feelsLike;
    AtmosphericForecast atmospheric;
    WindForecast wind;
    WeatherCondition weather;
} CurrentWeather;

typedef struct {
    int timestamp;
    float precipitation;
} MinutelyForecast;

typedef struct {
    int timestamp;
    float temperature;
    float feelsLike;
    AtmosphericForecast atmospheric;
    WindForecast wind;
    WeatherCondition weather;
    float pPrecipitation;
} HourlyForecast;

typedef struct {
    float day;
    float min;
    float max;
    float night;
    float eve;
    float morn;
} TemperatureForecast;

typedef struct {
    int timestamp;
    TemperatureForecast temperature;
    TemperatureForecast feelsLike;
    AtmosphericForecast atmospheric;
    WindForecast wind;
    WeatherCondition weather;
    float pPrecipitation;
    float rainPrecipitation;
    float snowPrecipitation;
} DailyForecast;

typedef struct {
    Coordinates coord;
    CurrentWeather current;
    MinutelyForecast minutely[NUM_MINUTELY]; // OpenWeather always provides up to 60 minutes of minutely data
    HourlyForecast hourly[NUM_HOURLY];     // OpenWeather provides up to 48 hours of hourly data
    DailyForecast daily[NUM_DAILY];        // OpenWeather provides up to 7 days of daily
} OpenWeatherData;


typedef struct {
    char ssid[MAX_SSID_LEN];
    char psk[MAX_PSK_LEN];
} wpa_info_t;