#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdint.h>
#include <vector>

#include "RBCXTimers.h"

#include "rbcx.pb.h"

namespace rb {

class Manager;

class Ultrasound {
    friend class Manager;

public:
    //!< Default is speed at 25C, 50%, 101 kPa
    static constexpr float defaultSpeedOfSound = 347.13f;

    typedef std::function<bool(uint32_t)> callback_t;

    /**
     * @brief Get the last measured distance in mm.
     * If the last measurement was invalid or not yet done, returns 0.
     * @return uint32_t
     */
    uint32_t lastDistanceMm() const { return m_lastDistanceMm; }

    /**
     * @brief Get the last measured duration in us.
     * If the last measurement was invalid or not yet done, returns 0.
     * @return uint32_t
     */
    uint32_t lastDurationUs() const { return m_lastUs; }

    /**
     * @brief Returns true if the last measurement was valid
     * Tests if the last measurement was`nt 0.
     * @return bool
     */
    bool isLastMeasurementValid() const { return m_lastUs != 0; }

    /**
     * @brief Set the speed of sound in m/s.
     */
    void setSpeedOfSound(float speedOfSoundInMetersPerSecond = defaultSpeedOfSound);

    /**
     * @brief Get the speed of sound in m/s.
     */
    float getSpeedOfSound() const { return m_speedOfSound; }

    /**
     * @brief Asynchronously measure the distance to an object.
     * The callback will be called when the measurement is done.
     * @param callback is a function which will be called when the measurement is done.
     */
    void measureAsync(callback_t callback = nullptr);

    /**
     * @brief Synchronously measure the distance to an object.
     * This function will block until the measurement is done.
     * @return uint32_t
     */
    uint32_t measure();

private:
    Ultrasound();
    Ultrasound(const Ultrasound&) = delete;
    ~Ultrasound();

    void init(uint8_t index);

    void recalculateLastDistanceLocked();
    void onMeasuringDone(const CoprocStat_UltrasoundStat& result);

    uint8_t m_index;

    uint32_t m_lastUs;
    uint32_t m_lastDistanceMm;
    float m_speedOfSound;
    bool m_measuring;
    std::vector<callback_t> m_callbacks;
    std::recursive_mutex m_mutex;
    std::condition_variable_any m_measuringDone;
    uint16_t m_timeoutTimer;
};

};
