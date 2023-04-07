#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

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
        delay(1000); // wait 1s
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
