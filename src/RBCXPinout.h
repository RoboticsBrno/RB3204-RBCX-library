#pragma once

#include <driver/adc.h>
#include <driver/gpio.h>

namespace rb {

enum class MotorId : uint8_t {
    M1,
    M2,
    M3,
    M4,

    MAX,
};

inline MotorId operator++(MotorId& x) {
    return x = MotorId(static_cast<uint8_t>(x) + 1);
}

} // namespace rb
