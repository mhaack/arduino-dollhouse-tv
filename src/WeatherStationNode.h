/*
* WeatherStationNode.h
* Homie node that fetches data from OpenWeatherMap service
* using ESP8266 Weather Station library.
*
* Version: 2.0
* Author: Markus Haack (http://github.com/mhaack)
*/

#pragma once

#include <HomieNode.hpp>
#include <OpenWeatherMapCurrent.h>

class WeatherStationNode : public HomieNode {
private:
    const boolean IS_METRIC = true;
    OpenWeatherMapCurrentData _currentWeather;
    unsigned long _nextUpdate;

    bool isConfigured();

protected:
    virtual void loop() override;

public:
    WeatherStationNode(const char* name);
    OpenWeatherMapCurrentData getCurrentWeather() const { return _currentWeather; }
    void setupHandler();
};