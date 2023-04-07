#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    while (true) {
        micros(); // update overflow
        man.setMotors()
            .power(rb::MotorId::M1, 32767) // motor power is from -32767 to 32767
            .power(rb::MotorId::M4, 32767) // motor power is from -32767 to 32767
            .set();
        printf("lmotor power: %d rmotor power: %d\n", 32767, 32767);
        delay(1000);

        rb::Manager::get()
            .setMotors()
            .power(rb::MotorId::M1, 0)
            .power(rb::MotorId::M4, 0)
            .set();
        printf("lmotor power: %d rmotor power: %d\n", 0, 0);
        delay(5000);
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
