#pragma once

#include <chrono>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ratio>

namespace rb {

template <typename T> T clamp(T value, T min, T max) {
    if (value < min) {
        return min;
    } else if (value > max) {
        return max;
    }
    return value;
}

inline void delayMs(int ms) { vTaskDelay(ms / portTICK_PERIOD_MS); }

inline void delay(std::chrono::duration<uint32_t, std::milli> delay) {
    vTaskDelay(delay.count() / portTICK_PERIOD_MS);
}

} // namespace rb
