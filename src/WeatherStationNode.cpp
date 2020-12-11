/*
* WeatherStationNode.cpp
* Homie node that fetches data from weather underground using ESP8266 Weather Station library.
*
* Version: 1.0
* Author: Markus Haack (http://github.com/mhaack)
*/

#include "WeatherStationNode.h"
#include <Homie.hpp>

// openWeatherMap Settings
HomieSetting<const char *> apiKey("WeatherApiKey", "Your OpenWeatherMap API key");
HomieSetting<const char *> language("WeatherLanguage", "The language in which you want to retrieve the weather data");
HomieSetting<const char *> location("WeatherLocation", "The OpenWeatherMap location id");
HomieSetting<long> updateInterval("WeatherUpdate", "The update interval in minutes");

OpenWeatherMapCurrent _openWeatherMapClient;

WeatherStationNode::WeatherStationNode(const char *name)
    : HomieNode(name, "openWeatherMap", "Weather") {}

void WeatherStationNode::setupHandler()
{
    Homie.getLogger() << "openWeatherMapNode - setupHandler" << endl;
    _openWeatherMapClient.setMetric(IS_METRIC);
    _openWeatherMapClient.setLanguage(language.get());
    _nextUpdate = millis() + 5000; // Wait 5 seconds before fetching weather underground data for the first time
};

bool WeatherStationNode::isConfigured()
{
    return apiKey.wasProvided() && location.wasProvided();
}

void WeatherStationNode::loop()
{
    if (isConfigured())
    {
        if (millis() >= _nextUpdate)
        {
            _openWeatherMapClient.updateCurrentById(&_currentWeather, apiKey.get(), location.get());
            _nextUpdate = millis() + updateInterval.get() * 60000UL;
            Homie.getLogger() << "openWeatherMapNode - updated weather data, next update at " << _nextUpdate << endl;
        }
    }
};