#pragma once

#include <atomic>
#include <functional>

#include <driver/gpio.h>
#include <driver/pcnt.h>

#include "RBCXPinout.h"
#include "RBCXUtil.h"

namespace rb {

class Encoder;
class Manager;

class Encoder {
    friend class Manager;
    friend class Motor;

public:
    ~Encoder();

    /**
     * \brief Drive motor to set position (according absolute value).
     *
     * \param positionAbsolute absolute position on which the motor drive \n
     *        e.g. if the actual motor position (`value()`) is 1000 and the `positionAbsolute` is 100
     *        then the motor will go backward to position 100
     * \param power maximal power of the motor when go to set position, allowed values: <0 - 100>
     * \param callback is a function which will be called after the motor arrives to set position `[optional]`
     */
    void driveToValue(int32_t positionAbsolute, uint8_t power,
        std::function<void(Encoder&)> callback = nullptr);
    /**
     * \brief Drive motor to set position (according relative value).
     *
     * \param positionRelative relative position on which the motor drive \n
     *        e.g. if the actual motor position (`value()`) is 1000 and the `positionRelative` is 100
     *        then the motor will go to position 1100
     * \param power maximal power of the motor when go to set position, allowed values: <0 - 100>
     * \param callback is a function which will be call after the motor arrive to set position `[optional]`
     */
    void drive(int32_t positionRelative, uint8_t power,
        std::function<void(Encoder&)> callback = nullptr);

    /**
     * \brief Get number of edges from encoder.
     * \return The number of counted edges from the first initialize
     *         of the encoder {@link Manager::initEncoder}
     */
    int32_t value();

    /**
     * \brief Get number of edges per one second.
     * \return The number of counted edges after one second.
     */
    float speed();

private:
    Encoder();
    Encoder(const Encoder&) = delete;

    void setId(MotorId id) { m_id = id; }

    MotorId m_id;
};

} // namespace rb
