#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>

#include "RBCXEncoder.h"
#include "RBCXPinout.h"
#include "RBCXUtil.h"

#include "rbcx.pb.h"

namespace rb {

class Manager;
class MotorChangeBuilder;

class Motor {
    friend class Manager;
    friend class MotorChangeBuilder;

public:
    /**
     * \brief Set motor power.
     * \param power of the motor <-32768; 32767>
     */
    void power(int16_t value);

    /**
     * \brief Set motor speed
     * \param ticksPerSecond speed of the motor <-32768; 32767>
     */
    void speed(int16_t ticksPerSecond);

    /**
     * \brief Start braking.
     * \param brakingPower braking power in <0, 32767>
     */
    void brake(uint16_t brakingPower);

    /**
     * \brief Limit the maximum PWM value. If you call pwmMaxPercent(70) and then
     * power(100), the motors will spin at 70% of maximum speed. This scales both the power
     * and speed set through Motor.
     * \param pct of the max value <0 - 100>
     */
    void pwmMaxPercent(int8_t percent);

    /**
     * \brief Get current maximum PWM percent value.
     */
    int8_t pwmMaxPercent() const { return m_pwm_max_percent; }

// TODO
#if 0
    /**
     * \brief Drive motor to set position (according absolute value). See {@link Encoder::driveToValue}.
     */
    void driveToValue(int32_t positionAbsolute, uint8_t power,
        std::function<void(Encoder&)> callback = nullptr);
    /**
     * \brief Drive motor to set position (according relative value). See {@link Encoder::drive}.
     */
    void drive(int32_t positionRelative, uint8_t power,
        std::function<void(Encoder&)> callback = nullptr);
#endif
    /**
     * \brief Get the Encoder instance for this motor. See {@link Encoder}.
     */
    Encoder& encoder() { return m_encoder; }

    /**
     * \brief Get the Encoder instance for this motor. Same as {@link encoder}.
     */
    Encoder& enc() { return encoder(); }

private:
    Motor();
    Motor(const Motor&) = delete;

    void setId(MotorId id) {
        m_id = id;
        m_encoder.setId(id);
    }

    void sendMotorReq(const CoprocReq_MotorReq& req);

    int16_t scale(int16_t val);

    Encoder m_encoder;
    CoprocReq_MotorReq m_lastReq;
    std::mutex m_mutex;
    MotorId m_id;
    int8_t m_pwm_max_percent;
    int16_t m_power;
    int16_t m_speed;
};

} // namespace rb
