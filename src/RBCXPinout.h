#pragma once

#include <driver/adc.h>
#include <driver/gpio.h>

namespace rb {

enum class MotorId : uint8_t {
    M1, //!< Motor 1
    M2, //!< Motor 2
    M3, //!< Motor 3
    M4, //!< Motor 4

    MAX, //!< Maximal motor ID
};

//!< Increments the motor ID
inline MotorId operator++(MotorId& x) {
    return x = MotorId(static_cast<uint8_t>(x) + 1);
}

//!< Decrements the motor ID
inline MotorId operator--(MotorId& x) {
    return x = MotorId(static_cast<uint8_t>(x) - 1);
}

static constexpr int UltrasoundsCount = 4;
static constexpr int StupidServosCount = 4;

} // namespace rb
