#include <iostream>

#include <Arduino.h>

#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(500);

    printf("Init manager\n");
    auto& man = rb::Manager::get();
    man.install();

    while (true) {
        micros(); // update overflow
        man.setMotors()
            .power(rb::MotorId::M1, 20)
            .power(rb::MotorId::M2, 20)
            .set();
        printf("lmotor power: %d rmotor power: %d\n", 20, 20);
        delay(1000);

        rb::Manager::get()
            .setMotors()
            .power(rb::MotorId::M1, 0)
            .power(rb::MotorId::M2, 0)
            .set();
        printf("lmotor power: %d rmotor power: %d\n", 0, 0);
        delay(5000);
    }
}

void loop() {}
