#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager


    while (true) { // set motor power to max when button is pressed and brake when it's not
        if (man.buttons().down()) {
            man.leds().green(true);
            man.motor(rb::MotorId::M1).power(32767); // motor power is from -32767 to 32767
        } else {
            man.leds().green(false);
            man.motor(rb::MotorId::M1).brake(32767); // braking power is from 0 to 32767
        }
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
