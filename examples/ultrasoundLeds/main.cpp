#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setLedsByDistance(int distance) {
    auto& leds = rb::Manager::get().leds();
    if(distance < 50) {
        leds.byId(rb::LedId::L1, false);
        leds.byId(rb::LedId::L2, false);
        leds.byId(rb::LedId::L3, false);
        leds.byId(rb::LedId::L4, false);
    } else if(distance < 80) {
        leds.byId(rb::LedId::L1, true);
        leds.byId(rb::LedId::L2, false);
        leds.byId(rb::LedId::L3, false);
        leds.byId(rb::LedId::L4, false);
    } else if(distance < 110) {
        leds.byId(rb::LedId::L1, true);
        leds.byId(rb::LedId::L2, true);
        leds.byId(rb::LedId::L3, false);
        leds.byId(rb::LedId::L4, false);
    } else if(distance < 140) {
        leds.byId(rb::LedId::L1, true);
        leds.byId(rb::LedId::L2, true);
        leds.byId(rb::LedId::L3, true);
        leds.byId(rb::LedId::L4, false);
    } else {
        leds.byId(rb::LedId::All, true);
    }
}


void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    const int ultrasoundId = 0; // ultrasound id

    while (true) {
        int distance = man.ultrasound(ultrasoundId).measure(); // measure distance in mm
        printf("Distance: %4d mm (%3d cm)\n", distance, distance/10); // print distance in mm and cm
        setLedsByDistance(distance);
        delay(100); // wait 100ms
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
