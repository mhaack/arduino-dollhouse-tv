/*
 * dollhouse-tv.cpp
 *
 * Mini dollhouse TV 
 *
 * Version: 1.0.0
 * Author: Markus Haack (http://github.com/mhaack)
 */

#define FW_NAME "mini-tv"
#define FW_VERSION "1.0.0"

#include "ButtonNode.hpp"
#include "DisplayNode.hpp"
#include "WeatherStationFonts.h"
#include "WundergroundNode.h"
#include "images.h"
#include <Homie.h>
#include <OLEDDisplayUi.h>
#include <SSD1306Wire.h>

// init time client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600, 60000);

// declaring method prototypes
void drawWelcome(OLEDDisplay* display);
void drawCat(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawStars(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawClock(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawWeather(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawJaMa(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawDino(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void loopHandler();
void setupHandler();

// init display
SSD1306Wire display(0x3c, D2, D1);
OLEDDisplayUi ui(&display);

// init nodes
DisplayNode displayNode("display", display, ui, timeClient);
WundergroundNode wundergroundNode("wunderground");
ButtonNode buttonNode("button", D4, []() {
    displayNode.toggleTransitionMode();
});

// cat
uint8_t count;
uint8_t catState = 0;
int8_t catPosX = -64;
uint8_t catPosY[4];

// dino
uint8_t dinoState = 0;
int8_t dinoPosX = -64;

// stars
struct starType {
    uint8_t state;
    int8_t posX;
    int8_t posY;
};
uint8_t maxStars = 24;
uint8_t starsSize[6];
const uint8_t starsStep = 2;
starType* starStruct;

// jama
int8_t moveX = 0;
int8_t moveY = 0;
ulong move;

// clock
int clockCenterX = 64;
int clockCenterY = 24;
int clockRadius = 23;

void setup()
{
    Serial.begin(115200);
    Serial << endl
           << endl;

    Homie_setFirmware(FW_NAME, FW_VERSION);
    Homie.disableResetTrigger();
    Homie.disableLedFeedback();
    Homie.setLoggingPrinter(&Serial);
    Homie.setSetupFunction(setupHandler);
    Homie.setLoopFunction(loopHandler);
    Homie.onEvent([](HomieEvent event) { displayNode.Event(event); });

    // display welcome
    drawWelcome(&display);

    // init display
    displayNode.addFrame(drawDino);
    displayNode.addFrame(drawCat);
    displayNode.addFrame(drawStars);
    displayNode.addFrame(drawClock);
    displayNode.addFrame(drawJaMa);
    displayNode.addFrame(drawWeather);
    //displayNode.enableStatusFrame(true);

    Homie.setup();
}

void loop()
{
    Homie.loop();
}

void drawWelcome(OLEDDisplay* display)
{
    display->init();
    display->clear();
    display->flipScreenVertically();
    display->setColor(WHITE);
    display->drawRect(10, 10, 108, 44);
    display->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display->setFont(ArialMT_Plain_24);
    display->drawString(40, display->height() / 2, "Mia");
    display->setFont(ArialMT_Plain_16);
    display->setTextAlignment(TEXT_ALIGN_CENTER);
    display->drawString(85, 14, "MINI");
    uint16_t strWidth = display->getStringWidth("MINI");
    display->drawHorizontalLine(85 - strWidth / 2, 32, strWidth);
    display->drawString(85, 34, "TV");
    display->display();
    delay(8000);
}

void drawCat(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
    // draw cat
    display->setColor(WHITE);
    display->drawXbm(x + catPosX, y + catPosY[catState], 64, 40, picsPointers[catState]);

    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_CENTER);

    // move cat
    catState++;
    if (catState >= 4) {
        catState = 0;
    }
    catPosX++;
    if (catPosX > 128) {
        catPosX = -64;
    }
}

void drawStars(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
    if (state->frameState == FIXED) {
        // draw stars
        display->setColor(WHITE);
        for (count = 0; count < maxStars; count++) {
            display->drawFastImage(x + starStruct[count].posX, y + starStruct[count].posY, starsSize[starStruct[count].state], starsSize[starStruct[count].state], starPointers[starStruct[count].state]);
        }

        // move stars
        for (count = 0; count < maxStars; count++) {
            starStruct[count].posX -= starsStep;
            if (starStruct[count].posX < 0)
                starStruct[count].posX = 128;
            ++starStruct[count].state;
            if (starStruct[count].state >= 6)
                starStruct[count].state = 0;
        }
    }
}

// from SSD1306ClockDemo.ino
void drawClock(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
    // Draw the clock face
    display->setColor(WHITE);
    display->drawCircle(clockCenterX + x, clockCenterY + y, 2);
    //
    //hour ticks
    for (int z = 0; z < 360; z = z + 30) {
        //Begin at 0° and stop at 360°
        float angle = z;
        angle = (angle / 57.29577951); //Convert degrees to radians
        int x2 = (clockCenterX + (sin(angle) * clockRadius));
        int y2 = (clockCenterY - (cos(angle) * clockRadius));
        int x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 8))));
        int y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 8))));
        display->drawLine(x2 + x, y2 + y, x3 + x, y3 + y);
    }

    // display second hand
    float angle = timeClient.getSeconds() * 6;
    angle = (angle / 57.29577951); //Convert degrees to radians
    int x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 5))));
    int y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 5))));
    display->drawLine(clockCenterX + x, clockCenterY + y, x3 + x, y3 + y);
    //
    // display minute hand
    angle = timeClient.getMinutes() * 6;
    angle = (angle / 57.29577951); //Convert degrees to radians
    x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 4))));
    y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 4))));
    display->drawLine(clockCenterX + x, clockCenterY + y, x3 + x, y3 + y);
    //
    // display hour hand
    angle = timeClient.getHours() * 30 + int((timeClient.getMinutes() / 12) * 6);
    angle = (angle / 57.29577951); //Convert degrees to radians
    x3 = (clockCenterX + (sin(angle) * (clockRadius - (clockRadius / 2))));
    y3 = (clockCenterY - (cos(angle) * (clockRadius - (clockRadius / 2))));
    display->drawLine(clockCenterX + x, clockCenterY + y, x3 + x, y3 + y);
}

void drawWeather(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
    display->setColor(WHITE);
    display->setFont(ArialMT_Plain_10);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
    display->drawString(58 + x, 5 + y, wundergroundNode.getWUClient()->getWeatherText());

    display->setFont(ArialMT_Plain_24);
    String temp = wundergroundNode.getWUClient()->getCurrentTemp() + "°C";
    display->drawString(58 + x, 15 + y, temp);

    display->setFont(Meteocons_Plain_42);
    String weatherIcon = wundergroundNode.getWUClient()->getTodayIcon();
    int weatherIconWidth = display->getStringWidth(weatherIcon);
    display->drawString(30 + x - weatherIconWidth / 2, 05 + y, weatherIcon);
}

void drawJaMa(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
    display->setColor(WHITE);
    display->drawXbm(x + moveX, y + moveY, jama_width, jama_height, jama_bits);
    if (state->frameState == FIXED && millis() >= move) {
        moveX = random(-5, 5);
        moveY = random(-5, 5);
        move = millis() + 1000UL;
    }
}

void drawDino(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y)
{
    display->setColor(WHITE);
    display->drawXbm(x + dinoPosX, y, dino_width, dino_height, dinoPointers[dinoState]);

    dinoState++;
    if (dinoState >= 16) {
        dinoState = 0;
    }
    dinoPosX++;
    if (dinoPosX > 128) {
        dinoPosX = -64;
    }
}

void setupHandler()
{
    Homie.getLogger() << "Setup handler" << endl;

    // init cat
    for (count = 0; count < 5; count++) {
        catPosY[count] = count + 9;
    }

    // init stars
    starsSize[0] = 1;
    starsSize[1] = 5;
    starsSize[2] = 7;
    starsSize[3] = 7;
    starsSize[4] = 5;
    starsSize[5] = 3;
    while ((maxStars > 0) && (starStruct == NULL)) {
        if ((starStruct = (starType*)malloc(sizeof(starType) * maxStars)) == NULL)
            --maxStars;
    }
    for (count = 0; count < maxStars; count++) {
        starStruct[count] = { uint8_t(random(0, 6)), int8_t(random(0, 128)), int8_t(random(0, 60)) };
    }

    move = millis();
    timeClient.begin();
    wundergroundNode.setupHandler();
}

void loopHandler()
{
    timeClient.update();
}
