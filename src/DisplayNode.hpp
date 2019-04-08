/*
* DisplayNode.hpp
* Homie Node for SSD1306 OLED displays using ESP8266_SSD1306 display driver
* library.
*
* Version: 1.0
* Author: Markus Haack (http://github.com/mhaack)
*/

#pragma once

#include <HomieNode.hpp>
#include <NTPClient.h>
#include <OLEDDisplayUi.h>
#include <SSD1306Wire.h>

class DisplayNode : public HomieNode {
    static const uint8_t MAXFRAMES = 7;

public:
    DisplayNode(const char* name, SSD1306Wire& display, OLEDDisplayUi& ui, NTPClient& timeClient);

    uint8_t addFrame(FrameCallback frame);
    bool toggleTransitionMode();
    void enableStatusFrame(bool enabled) { statusEnabled = enabled; }
    void Event(HomieEvent event);

protected:
    virtual void setup() override;
    virtual void loop() override;

private:
    const char* cIndent = "  ◦ ";

    SSD1306Wire display;
    OLEDDisplayUi ui;
    NTPClient& timeClient;
    FrameCallback frames[MAXFRAMES];
    OverlayCallback overlays[1];
    uint8_t frameCount = 0;
    bool statusEnabled = false;
    bool autoTransition = true;

    static void drawOverlay(OLEDDisplay* display, OLEDDisplayUiState* state);
    static void drawStatusFrame(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
};
