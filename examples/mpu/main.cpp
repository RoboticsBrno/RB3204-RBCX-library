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
    mpu.sendStart();

    while (true) {
        printf("MPU - angle: X: %2.2f Y: %2.2f Z: %2.2f\n", mpu.getAngleX(), mpu.getAngleY(), mpu.getAngleZ());
        delay(100);
    }

}

void loop() {}