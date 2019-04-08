/*
* WundergroundNode.h
* Homie node that fetches data from weather underground using ESP8266 Weather Station library.
*
* Version: 1.0
* Author: Markus Haack (http://github.com/mhaack)
*/

#pragma once

#include <HomieNode.hpp>
#include <WundergroundClient.h>

class WundergroundNode : public HomieNode {
private:
    const boolean IS_METRIC = true;
    WundergroundClient* _wunderground;
    unsigned long _nextUpdate;

    bool isConfigured();

protected:
    virtual void loop() override;

public:
    WundergroundNode(const char* name);
    WundergroundClient* getWUClient() const { return _wunderground; }
    void setupHandler();
};