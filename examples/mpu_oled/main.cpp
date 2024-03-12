#include <Arduino.h>
#include "RBCX.h"

#define D_WIDTH 128
#define D_HEIGHT 64

/**
 * @brief Show ball in the center of the screen and move it with y and z axis
 * @param oled OLED display
 * @param angle MPU angle
*/
void showCenterGyroBall(rb::Oled& oled, rb::Mpu &mpu) {
    // show ball in the ceter of the screen - move with x an y axis
    auto angle = mpu.getAngle();
    oled.fill(rb::Oled::Black);
    int x = D_WIDTH / 2 + angle.z * 2;
    int y = D_HEIGHT / 2 + angle.y * 2;
    oled.drawCircle(x, y, 5, rb::Oled::White);
    oled.updateScreen();
}

/**
 * @brief Show gyro info on the screen
 * @param oled OLED display
 * @param angle MPU angle
*/
void showGyroIInfo(rb::Oled& oled, rb::Mpu &mpu) {
    auto angle = mpu.getAngle();
    oled.fill(rb::Oled::Black);
    oled.setCursor(0, 0);
    oled.writeString(String(angle.x).c_str(), rb::Oled::Font_11x18, rb::Oled::White);
    oled.setCursor(0, 20);
    oled.writeString(String(angle.y).c_str(), rb::Oled::Font_11x18, rb::Oled::White);
    oled.setCursor(0, 40);
    oled.writeString(String(angle.z).c_str(), rb::Oled::Font_11x18, rb::Oled::White);
    oled.updateScreen();
}

void setup() {
    printf("RB3204-RBCX\n");

    delay(500);

    printf("Init manager\n");
    auto& man = rb::Manager::get();
    man.install();

    man.leds().blue(true);

    auto &oled = rb::Manager::get().oled();
    auto &mpu = rb::Manager::get().mpu();
    mpu.init();

    mpu.calibrateNow();
    mpu.sendStart();

    oled.init(rb::Oled::Oled_128x64, true, false);

    while (true) {
        auto angle = mpu.getAngle();
        printf("MPU - angle: X: %2.2f Y: %2.2f Z: %2.2f\n", angle.x, angle.y, angle.z);

        // every 3 swithc between show ball and show info
        if (millis() % 10000 < 5000) {
            showCenterGyroBall(oled, mpu);
        } else {
            showGyroIInfo(oled, mpu);
        }

        delay(50);
    }
}

void loop() {}