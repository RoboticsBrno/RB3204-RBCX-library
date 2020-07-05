#include "RBCXMotor.h"
#include "RBCXEncoder.h"
#include "RBCXManager.h"

#include "rbcx.pb.h"

namespace rb {

Motor::Motor() {
    m_power = 0;
    m_speed = 0;
    m_lastReq = CoprocReq_MotorReq_init_zero;
    m_pwm_max_percent = 100;
}

void Motor::power(int16_t value) {
    sendMotorReq(CoprocReq_MotorReq {
        .motorIndex = (uint32_t)m_id,
        .which_motorCmd = CoprocReq_MotorReq_setPower_tag,
        .motorCmd = {
            .setPower = scale(value),
        },
    });
}

void Motor::speed(int16_t ticksPerSecond) {
    sendMotorReq(CoprocReq_MotorReq {
        .motorIndex = (uint32_t)m_id,
        .which_motorCmd = CoprocReq_MotorReq_setVelocity_tag,
        .motorCmd = {
            .setVelocity = scale(ticksPerSecond),
        },
    });
}

void Motor::brake(uint16_t value) {
    sendMotorReq(CoprocReq_MotorReq {
        .motorIndex = (uint32_t)m_id,
        .which_motorCmd = CoprocReq_MotorReq_setBrake_tag,
        .motorCmd = {
            .setBrake = value,
        },
    });
}

void Motor::pwmMaxPercent(int8_t percent) {
    m_pwm_max_percent = std::min(int8_t(100), std::max(int8_t(0), percent));
}

void Motor::driveToValue(int32_t positionAbsolute, uint8_t power,
    std::function<void(Encoder&)> callback) {
    m_encoder.driveToValue(positionAbsolute, power, callback);
}

void Motor::drive(int32_t positionRelative, uint8_t power,
    std::function<void(Encoder&)> callback) {
    m_encoder.drive(positionRelative, power, callback);
}

int16_t Motor::scale(int16_t val) {
    return int32_t(val) * m_pwm_max_percent / 100;
}

void Motor::sendMotorReq(const CoprocReq_MotorReq& req) {
    std::lock_guard<std::mutex> l(m_mutex);

    if (memcmp(&m_lastReq, &req, sizeof(CoprocReq_MotorReq)) == 0)
        return;
    memcpy(&m_lastReq, &req, sizeof(CoprocReq_MotorReq));

    const CoprocReq coprocReq = {
        .which_payload = CoprocReq_motorReq_tag,
        .payload = {
            .motorReq = req,
        },
    };
    Manager::get().sendToCoproc(coprocReq);
}

}; // namespace rb
