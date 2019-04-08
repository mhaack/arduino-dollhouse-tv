/*
* WundergroundNode.cpp
* Homie node that fetches data from weather underground using ESP8266 Weather Station library.
*
* Version: 1.0
* Author: Markus Haack (http://github.com/mhaack)
*/

#include "WundergroundNode.h"
#include <Homie.hpp>

// Wunderground Settings
HomieSetting<const char*> wundergroundApiKey("WuApiKey", "Your weather underground API key");
HomieSetting<const char*> wundergroundLanguage("WuLanguage", "The language in which you want to retrieve the weather underground data");
HomieSetting<const char*> wundergroundCountry("WuCountry", "The country in which your citie lies");
HomieSetting<const char*> wundergroundCity("WuCity", "The city for which you want to retrieve the weather underground data");
HomieSetting<long> wundergroundUpdate("WuUpdate", "The update interval in minutes for weather underground (must be at least 10 minutes)");

WundergroundNode::WundergroundNode(const char* name)
    : HomieNode(name, "Wunderground")
{
    _wunderground = new WundergroundClient(IS_METRIC);
    _nextUpdate = 0;
}

void WundergroundNode::setupHandler()
{
    Homie.getLogger() << "WundergroundNode - Setuphandler" << endl;
    _nextUpdate = millis() + 5000; // Wait 5 seconds before fetching weather underground data for the first time
};

bool WundergroundNode::isConfigured()
{
    return wundergroundApiKey.wasProvided() && wundergroundCity.wasProvided();
}

void WundergroundNode::loop()
{
    if (isConfigured()) {
        if (millis() >= _nextUpdate) {
            _wunderground->updateConditions(wundergroundApiKey.get(), wundergroundLanguage.get(), wundergroundCountry.get(), wundergroundCity.get());
            _nextUpdate = millis() + wundergroundUpdate.get() * 60000UL;
        }
    }
};