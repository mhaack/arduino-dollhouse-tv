/*
 * DisplayNode.cpp
 * Homie Node for SSD1306 OLED displays using ESP8266_SSD1306 display driver
 * library.
 *
 * Version: 1.0
 * Author: Markus Haack (http://github.com/mhaack)
 */

#include "DisplayNode.hpp"
#include "icons.h"

#include <ESP8266WiFi.h>
#include <Homie.hpp>

HomieEvent homieEvent;

HomieSetting<bool> flipScreenSetting("flipScreen", "Flip the display vertically");

DisplayNode::DisplayNode(const char* name, SSD1306Wire& display, OLEDDisplayUi& ui, NTPClient& timeClient)
    : HomieNode(name, "OLEDDisplay")
    , display(display)
    , ui(ui)
    , timeClient(timeClient)
    , statusEnabled(false)
{
}

void DisplayNode::setup()
{
    // init dispaly
    ui.getUiState()->userData = this;
    ui.disableAllIndicators();
    ui.setTargetFPS(30);
    ui.setFrameAnimation(SLIDE_LEFT);
    ui.setTimePerFrame(30000);
    ui.setTimePerTransition(1000);
    ui.enableAutoTransition();
    //ui.disableAutoTransition();

    // Add frames
    if (statusEnabled) {
        addFrame(DisplayNode::drawStatusFrame);
    }
    ui.setFrames(frames, frameCount);

    // Add overlays
    static OverlayCallback overlays[] = { DisplayNode::drawOverlay };
    static uint8_t overlaysCount = 1;
    ui.setOverlays(overlays, overlaysCount);

    // Initialising the UI will init the display too.
    ui.init();

    bool flipScreen = flipScreenSetting.get();
    if (flipScreen) {
        display.flipScreenVertically();
    }

    Homie.getLogger() << cIndent << "OLED DisplayNode setup successful!" << endl;
}

void DisplayNode::loop() { ui.update(); }

void DisplayNode::Event(HomieEvent event) { homieEvent = event; }

uint8_t DisplayNode::addFrame(FrameCallback frame)
{
    frames[frameCount++] = frame;
    return frameCount;
}

bool DisplayNode::toggleTransitionMode()
{
    if (autoTransition) {
        ui.disableAutoTransition();
        autoTransition = false;
    } else {
        ui.nextFrame();
        ui.enableAutoTransition();
        autoTransition = true;
    }
    return autoTransition;
}

// converts the dBm to a range between 0 and 100%
static int8_t getWifiQuality()
{
    int32_t dbm = WiFi.RSSI();

    if (dbm <= -100) {
        return 0;
    } else if (dbm >= -50) {
        return 100;
    } else {
        return 2 * (dbm + 100);
    }
}

void DisplayNode::drawStatusFrame(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
    if ((state == NULL) || (state->userData == NULL)) {
        return;
    }
    const DisplayNode* self = static_cast<DisplayNode*>(state->userData);

    if (self != NULL) {
        display->setTextAlignment(TEXT_ALIGN_CENTER);
        display->setFont(ArialMT_Plain_10);
        display->drawString(64 + x, 0 + y, "System Status");
        display->drawHorizontalLine(0 + x, 13 + y, 128);

        int8_t wifiQuality = getWifiQuality();
        for (int8_t i = 0; i < 4; i++) {
            for (int8_t j = 0; j < 2 * (i + 1); j++) {
                if ((wifiQuality > i * 25) || (j == 0)) {
                    display->setPixel(x + (120 + 2 * i), 10 - j);
                }
            }
        }

        display->setTextAlignment(TEXT_ALIGN_LEFT);
        display->drawString(0 + x, 13 + y, "IP: " + homieEvent.ip.toString());
        const String time = self->timeClient.getFormattedTime();
        display->drawString(0 + x, 24 + y, "Time: " + time);
    }
}

void DisplayNode::drawOverlay(OLEDDisplay* display, OLEDDisplayUiState* state)
{
    if ((state == NULL) || (state->userData == NULL)) {
        return;
    }
    const DisplayNode* self = static_cast<DisplayNode*>(state->userData);

    if (self != NULL) {

        display->setColor(self->autoTransition ? BLACK : WHITE);
        display->fillCircle(display->getWidth(), display->height(), 12);
        display->setColor(self->autoTransition ? WHITE : BLACK);
        if (self->autoTransition) {
            display->drawCircleQuads(display->getWidth(), display->height(), 12, 0b00000010);
        }
        display->setTextAlignment(TEXT_ALIGN_RIGHT);
        display->setFont(ArialMT_Plain_10);
        display->drawString(display->getWidth(), display->height() - 11, (String)(state->currentFrame + 1));
    }
}
