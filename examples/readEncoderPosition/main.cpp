#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    while (true) { // each 100ms request motor info and print motor position
        man.motor(rb::MotorId::M1).requestInfo([](rb::Motor& info) {
            printf("M1: position:%d\n", info.position());
        });
        delay(100);
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
