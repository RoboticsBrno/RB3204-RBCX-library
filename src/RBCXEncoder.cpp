#include <driver/gpio.h>
#include <driver/pcnt.h>
#include <driver/periph_ctrl.h>
#include <esp_log.h>

#include "RBCXEncoder.h"
#include "RBCXManager.h"
#include "RBCXPinout.h"

#define TAG "RbEncoder"

namespace rb {

Encoder::Encoder() {}

Encoder::~Encoder() {}

#if 0
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

#endif

};
