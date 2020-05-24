#include "RBCXMotor.h"
#include "RBCXEncoder.h"
#include "RBCXManager.h"

namespace rb {

Motor::Motor(Manager& man, MotorId id)
    : m_man(man)
    , m_id(id) {
    m_power = 0;
    m_pwm_max_percent = 0;
}

void Motor::stop() {
    // TODO
}

void Motor::power(int8_t value) {
    // TODO
}

void Motor::pwmMaxPercent(int8_t percent) {
    // TODO
}

void Motor::driveToValue(int32_t positionAbsolute, uint8_t power,
    std::function<void(Encoder&)> callback) {
    encoder()->driveToValue(positionAbsolute, power, callback);
}

void Motor::drive(int32_t positionRelative, uint8_t power,
    std::function<void(Encoder&)> callback) {
    encoder()->drive(positionRelative, power, callback);
}

Encoder* Motor::encoder() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_encoder) {
        m_encoder.reset(new Encoder(m_man, m_id));
    }
    return m_encoder.get();
}

}; // namespace rb
