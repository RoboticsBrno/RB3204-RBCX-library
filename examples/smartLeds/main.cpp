#include "RBCX.h"
#include "SmartLeds.h" // Add SmartLeds library to platformio.ini - lib_deps = https://github.com/RoboticsBrno/SmartLeds/
#include <Arduino.h>
#include <iostream>
#include <memory>

const int DATA_PIN = 12; // data LED pin on RBCX board
const int LED_COUNT = 8; // number of LEDs

void showRgb(SmartLed& leds) {
    leds[0] = Rgb { 255, 0, 0 };
    leds[1] = Rgb { 0, 255, 0 };
    leds[2] = Rgb { 0, 0, 255 };
    leds[3] = Rgb { 0, 0, 0 };
    leds[4] = Rgb { 128, 128, 128 };
    leds[5] = Rgb { 128, 0, 0 };
    leds[6] = Rgb { 0, 128, 0 };
    leds[7] = Rgb { 0, 0, 128 };
    leds.show();
}

void showGradient(SmartLed& leds) {
    static uint8_t hue = 0; // static variable is initialized only once
    hue+=15;
    // Use HSV to create nice gradient
    for (int i = 0; i != LED_COUNT; i++)
        leds[i] = Hsv { static_cast<uint8_t>(hue + 30 * i), 255, hue%254 };
        // HSV - Hue (color), Saturation (color intensity), Value (brightness)
    leds.show();
}


void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    SmartLed leds(LED_WS2812B, LED_COUNT, DATA_PIN, 0, DoubleBuffer);

    while (true) {
        if (millis() % 10000 < 5000)
            showGradient(leds);
        else
            showRgb(leds);
        delay(50);
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
