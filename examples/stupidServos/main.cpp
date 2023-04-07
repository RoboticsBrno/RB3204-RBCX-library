#include <iostream>
#include <Arduino.h>
#include "RBCX.h"

void setup() {
    printf("RB3204-RBCX\n");
    delay(50);

    printf("Init manager\n");
    auto& man = rb::Manager::get(); // get manager instance as singleton
    man.install(); // install manager

    const int servoId = 0; // servo id
    const float maxRange = 2.0f; // my servo accepts values from -2 to 2 (Servo SG-90 9g - range 0-180°)

    while (true) {
        if(man.buttons().down()) {
            man.stupidServo(servoId).setPosition(-maxRange); // on button down, set servo to min range
        } else if(man.buttons().up()) {
            man.stupidServo(servoId).setPosition(maxRange); // on button up, set servo to max range
        } else {
            man.stupidServo(servoId).setPosition(0); // default center position
        }
    }
}

void loop() {} // I don't need loop, because i'm using while(true) in setup (it doesn't require to create global variables)
