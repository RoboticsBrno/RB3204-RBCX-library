#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    while (true) { // enable buzzer + yellow led if button DOWN is pressed
        if (man.buttons().down()) {
            man.leds().yellow(true);
            man.piezo().start();
        } else {
            man.leds().yellow(false);
            man.piezo().stop();
        }
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
