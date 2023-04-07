#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    int i = 0;
    const auto& bat = man.battery();
    while(true) { // each 1s print battery status
        printf("Tick #%d, battery at %d%%, %dmv\n", i++, bat.pct(), bat.voltageMv());
        delay(1000);
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
