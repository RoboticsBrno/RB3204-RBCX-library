#pragma once

#include <atomic>
#include "WString.h"

namespace rb {

/**
 * \brief Helper class for controlling the Oled.
 */
class Oled {
    friend class Manager;

public:
    typedef enum {
        Oled_128x32,       
        Oled_128x64,       
    } OledType;

    // Enumeration for screen colors
    typedef enum OledColor{
        Black = 0, // Black color, no pixel
        White = 1  // Pixel is set. Color depends on OLED
    } OledColor;

    typedef enum {
        Font_6x8,
        Font_7x10,
        Font_11x18,
        Font_16x26,        
    } OledFontDef;


    void init(OledType type, bool rotate = true, bool inverseColor = false);
    void fill(OledColor color = White);
    void updateScreen(void);
    void drawPixel(uint8_t x, uint8_t y, OledColor color = White);
    void writeString(const char* str, OledFontDef Font, OledColor color = White);
    void writeString(char* str, OledFontDef Font, OledColor color = White);
    void writeString(String str, OledFontDef Font, OledColor color = White);
    void setCursor(uint8_t x, uint8_t y);
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OledColor color);
    void drawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, OledColor color);
    void drawCircle(uint8_t x, uint8_t y, uint8_t radius, OledColor color);
    void drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OledColor color);
    inline uint8_t getWidth() {return m_width;}
    inline uint8_t getHeight() {return m_height;}

private:
    Oled();
    Oled(const Oled&) = delete;
    ~Oled();
    void sendOledReq(CoprocReq_OledReq oledReq);

    bool m_init;
    uint32_t m_width;
    uint32_t m_height;

}; // class Oled
}; // namespace rb
