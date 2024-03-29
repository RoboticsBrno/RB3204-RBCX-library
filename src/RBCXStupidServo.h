#pragma once

#include <mutex>
#include <stdint.h>

#include "rbcx.pb.h"

namespace rb {

class Manager;

class StupidServo {
    friend class Manager;

public:
    uint8_t id() const { return m_id; }

    //!< set servo posiotion, range is <0;1>. Some servos might accept a bit more
    void setPosition(float positionFraction);

    //!< Returns last set position (can be NaN if disabled)
    float position() const { return m_lastPosition; }

    //!< Disables servo
    void disable();

private:
    StupidServo(uint8_t id);
    ~StupidServo() {}
    StupidServo(const StupidServo&) = delete;

    void sendServoMsg(float position);

    std::mutex m_mutex;
    float m_lastPosition;
    uint8_t m_id;
};

};
