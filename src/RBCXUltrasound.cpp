#include <esp_log.h>

#include "RBCXManager.h"
#include "RBCXUltrasound.h"
#include "rbcx.pb.h"

#define TAG "RBCXUltrasound"

namespace rb {

Ultrasound::Ultrasound()
    : m_index(0)
    , m_lastUs(0)
    , m_lastDistanceMm(0)
    , m_speedOfSound(defaultSpeedOfSound / 100.f)
    , m_measuring(false)
    , m_timeoutTimer(0) {

    m_timeoutTimer = Timers::get().schedule(0xFFFFFFFF, [this]() -> bool {
        ESP_LOGE(TAG, "Ultrasound response timeout!\n");
        onMeasuringDone(CoprocStat_UltrasoundStat { 0 });
        return false;
    });

    Timers::get().stop(m_timeoutTimer);
}

Ultrasound::~Ultrasound() {}

void Ultrasound::setSpeedOfSound(float speedOfSoundInMetersPerSecond) {
    m_mutex.lock();
    m_speedOfSound = speedOfSoundInMetersPerSecond / 100.f;
    recalculateLastDistanceLocked();
    m_mutex.unlock();
}

void Ultrasound::measureAsync(std::function<void(uint32_t)> callback) {
    std::lock_guard<std::recursive_mutex> ul(m_mutex);

    if (!m_measuring) {
        m_measuring = true;
        Manager::get().sendToCoproc(
            CoprocReq { .which_payload = CoprocReq_ultrasoundReq_tag,
                .payload
                = { .ultrasoundReq = {
                        .utsIndex = m_index,
                        .which_utsCmd = CoprocReq_UltrasoundReq_singlePing_tag,
                    } } });
        Timers::get().reset(m_timeoutTimer, 250);
    }

    if (callback)
        m_callbacks.emplace_back(callback);
}

uint32_t Ultrasound::measure() {
    std::unique_lock<std::recursive_mutex> ul(m_mutex);
    measureAsync();
    m_measuringDone.wait(ul);
    return m_lastDistanceMm;
}

void Ultrasound::onMeasuringDone(const CoprocStat_UltrasoundStat& result) {
    std::vector<callback_t> callbacks;
    uint32_t distance;

    {
        std::lock_guard<std::recursive_mutex> ul(m_mutex);

        m_lastUs = result.roundtripMicrosecs;
        recalculateLastDistanceLocked();
        distance = m_lastDistanceMm;
        m_measuring = false;
        m_callbacks.swap(callbacks);
        Timers::get().stop(m_timeoutTimer);
        m_measuringDone.notify_all();
    }

    for (const auto& cb : callbacks) {
        cb(distance);
    }
}

void Ultrasound::recalculateLastDistanceLocked() {
    if (m_lastUs == 0) {
        m_lastDistanceMm = 0;
        return;
    }

    m_lastDistanceMm = uint32_t(m_speedOfSound * (float(m_lastUs) / 100.f)) / 2;
}

};
