#include <driver/gpio.h>
#include <driver/pcnt.h>
#include <driver/periph_ctrl.h>
#include <esp_log.h>

#include "RBCXEncoder.h"
#include "RBCXManager.h"
#include "RBCXPinout.h"

#define TAG "RbEncoder"

namespace rb {

Encoder::Encoder(rb::Manager& man, rb::MotorId id)
    : m_manager(man)
    , m_id(id) {
    if (m_id >= MotorId::MAX) {
        ESP_LOGE(TAG, "Invalid encoder index %d, using 0 instead.", (int)m_id);
        m_id = MotorId::M1;
    }
}

Encoder::~Encoder() {}

int32_t Encoder::value() {
    // TODO
    return 0;
}

float Encoder::speed() {
    // TODO
    return 0;
}

void Encoder::driveToValue(int32_t positionAbsolute, uint8_t power,
    std::function<void(Encoder&)> callback) {
    // TODO
}

void Encoder::drive(int32_t positionRelative, uint8_t power,
    std::function<void(Encoder&)> callback) {
    // TODO
}

};
