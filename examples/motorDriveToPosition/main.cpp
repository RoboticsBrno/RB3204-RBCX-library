#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager


    while (true) { // drive motor M1 to position 1000 with 100% power (32767) if button down is pressed and return to 0 position if button is released
        if (man.buttons().down()) {
            man.leds().green(true);
            man.setMotors().driveToValue(rb::MotorId::M1, 1000, 32767).set();
        } else {
            man.leds().green(false);
            man.setMotors().driveToValue(rb::MotorId::M1, 0, 32767).set();
        }
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
