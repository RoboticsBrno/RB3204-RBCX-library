#pragma once

#include "BusBackend.hpp"
#include "rbcx.pb.h"
#include <mutex>

namespace rb {

class Manager;

class SmartServoBusBackend : public lx16a::BusBackend {
    friend class Manager;

public:
    virtual void send(const lw::Packet& pkt, QueueHandle_t responseQueue = NULL,
        bool expect_response = false, bool priority = false);

private:
    SmartServoBusBackend();
    SmartServoBusBackend(const SmartServoBusBackend&) = delete;
    ~SmartServoBusBackend();

    void onCoprocStat(const CoprocStat_SmartServoStat& msg);

    QueueHandle_t m_responseQueueQueue;
};

};
