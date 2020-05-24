#pragma once

#include <atomic>

#include <esp_adc_cal.h>

#include "RBCXLeds.h"
#include "RBCXPiezo.h"

namespace rb {

class Manager;

/**
 * \brief Contains the battery state and can control the robot's power.
 */
class Battery {
    friend class Manager;

public:
    static constexpr uint32_t VOLTAGE_MIN = 3300
        * 2; //!< Minimal battery voltage, in mV, at which the robot shuts down
    static constexpr uint32_t VOLTAGE_MAX
        = 4200 * 2; //!< Maximal battery voltage, in mV

    uint32_t pct() const; //!< returns current battery percentage
    uint32_t voltageMv() const; //!< returns current battery voltage

    void shutdown(); //!< shuts the robot down
private:
    Battery();
    Battery(const Battery&) = delete;
    ~Battery();

    std::atomic<uint32_t> m_voltageMv;
};

};
