#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    man.leds().red(true);
    man.leds().yellow(true);

    while (true) { // blink green and blue LEDs depending on DOWN button state
        if (man.buttons().down()) {
            man.leds().green(false);
            man.leds().blue(true);
        } else {
            man.leds().green(true);
            man.leds().blue(false);
        }
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
