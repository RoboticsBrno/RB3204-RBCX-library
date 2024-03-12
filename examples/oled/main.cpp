#include <Arduino.h>
#include "RBCX.h"

void clear(){
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
    
    auto& oled = rb::Manager::get().oled();

    oled.init(rb::Oled::Oled_128x64, true, false);
    // oled.init(rb::Oled::Oled_128x32, true, false);

    while (true) {
        ///////// fill ////////////////////////
        printf("fill\n");
        oled.fill(rb::Oled::White);
        oled.updateScreen();
        delay(1000);

        oled.fill(rb::Oled::Black);
        oled.updateScreen();
        delay(1000);


        ///////// drawPixel ////////////////////////
        printf("drawPixel\n");
        for(int i = 0; i<500; i++) {
            oled.drawPixel(random(0,oled.getWidth()), random(0, oled.getHeight()), rb::Oled::White);
        }
        oled.updateScreen();
        waitToNextTest();


        ///////// setCursor ////////////////////////
        ///////// writeString ////////////////////////
        printf("setCursor + writeString\n");
        oled.setCursor(45, 0);
        oled.writeString("OLED", rb::Oled::Font_11x18, rb::Oled::White);
        
        oled.setCursor(5, oled.getHeight()/2);
        String text = "OLED w:" + String(oled.getWidth()) + " | h:" + String(oled.getHeight());
        oled.writeString(text.c_str(), rb::Oled::Font_7x10, rb::Oled::White);
        oled.updateScreen();        
        waitToNextTest();


        ///////// drawLine ////////////////////////
        printf("drawLine\n");
        oled.drawLine(0, 0, oled.getWidth(), oled.getHeight(), rb::Oled::White);
        oled.drawLine(0, oled.getHeight(), oled.getWidth(), 0, rb::Oled::White);
        oled.updateScreen();
        waitToNextTest();


        ///////// drawArc ////////////////////////
        printf("drawArc\n");
        oled.drawArc(oled.getWidth()/2, oled.getHeight()/2, 15, 20, 270, rb::Oled::White);
        oled.updateScreen();
        waitToNextTest();


        ///////// drawCircle ////////////////////////
        printf("drawCircle\n");
        for(uint32_t delta = 0; delta < 5; delta ++) {
            oled.drawCircle(20* delta+30, 15, 10, rb::Oled::White);
        }
        oled.updateScreen();
        waitToNextTest();

        ///////// drawRectangle ////////////////////////
        printf("drawRectangle\n");
        for(int i = 5; i< oled.getWidth(); i+=5) {
            oled.drawRectangle(i, 0, i+i, oled.getHeight()-1, rb::Oled::White);
        }
        oled.updateScreen();      
        waitToNextTest();

    }
}

void loop() {}
