#include <esp_log.h>

#include "RBCXBattery.h"
#include "RBCXManager.h"

#define TAG "RBCXBattery"

namespace rb {

Battery::Battery() {}

Battery::~Battery() {}

void Battery::shutdown() {
    ESP_LOGW(TAG, "Shutting down.");

    vTaskDelay(pdMS_TO_TICKS(500));

    // TODO

    // Shut down nearly everything and never wake up - necessary when ESP is
    // powered from USB
    esp_deep_sleep_start();
}

uint32_t Battery::voltageMv() const { return m_voltageMv.load(); }

uint32_t Battery::pct() const {
    const auto mv = voltageMv();
    if (mv <= VOLTAGE_MIN) {
        return 0;
    } else if (mv >= VOLTAGE_MAX) {
        return 100;
    } else {
        return (float(mv - VOLTAGE_MIN) / (VOLTAGE_MAX - VOLTAGE_MIN)) * 100.f;
    }
}

};
