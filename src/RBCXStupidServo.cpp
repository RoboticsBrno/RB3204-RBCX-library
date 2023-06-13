#include "RBCXStupidServo.h"
#include "RBCXManager.h"

namespace rb {

StupidServo::StupidServo(uint8_t id)
    : m_lastPosition(NAN), m_id(id) {}

void StupidServo::setPosition(float posFraction) { sendServoMsg(posFraction); }

void StupidServo::disable() { sendServoMsg(NAN); }

void StupidServo::sendServoMsg(float position) {
    std::lock_guard<std::mutex> l(m_mutex);
    if (m_lastPosition == position)
        return;
    m_lastPosition = position;

    const pb_size_t tag = std::isnan(position)
        ? CoprocReq_SetStupidServo_disable_tag
        : CoprocReq_SetStupidServo_setPosition_tag;

    Manager::get().sendToCoproc(CoprocReq {
        .which_payload = CoprocReq_setStupidServo_tag,
        .payload = {
            .setStupidServo = {
                .servoIndex = m_id,
                .which_servoCmd = tag,
                .servoCmd = {
                    .setPosition = position,
                },
            },
        },
    });
}
};
