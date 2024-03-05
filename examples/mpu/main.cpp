#include "RBCX.h"
#include <Arduino.h>
#include <string>

void clear() {
    rb::Manager::get().oled().fill(rb::Oled::Black);
    rb::Manager::get().oled().updateScreen();
}

void waitToNextTest() {
    delay(3000);
    clear();
}

void setup() {
    printf("RB3204-RBCX\n");

    delay(500);

    printf("Init manager\n");
    auto& man = rb::Manager::get();
    man.install();

    man.leds().red(true);

    auto& mpu = rb::Manager::get().mpu();

    mpu.init();

    // sets the 0 positions to current RBCX orientation
    mpu.calibrateNow();
    delay(1000);

    mpu.sendStart();

    while (true) {
        auto angle = mpu.getAngle();
        printf("MPU - angle: X: %2.2f Y: %2.2f Z: %2.2f\n", angle.x, angle.y, angle.z);
        delay(100);
    }

}

void loop() {}
