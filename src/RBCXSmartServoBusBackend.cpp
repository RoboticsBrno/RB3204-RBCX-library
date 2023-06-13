#include "RBCXSmartServoBusBackend.h"
#include "RBCXManager.h"

using namespace lx16a;

#define TAG "SmartServoBusBackend"

namespace rb {

SmartServoBusBackend::SmartServoBusBackend() {
    m_responseQueueQueue = xQueueCreate(1, sizeof(QueueHandle_t));
}

SmartServoBusBackend::~SmartServoBusBackend() {
    vQueueDelete(m_responseQueueQueue);
}

void SmartServoBusBackend::send(const lw::Packet& pkt,
    QueueHandle_t responseQueue, bool expect_response, bool priority) {

    if(pkt._data.size() > sizeof(CoprocReq_SmartServoReq_data_t::bytes)) {
        ESP_LOGE(TAG, "SmartServo packet too big, %d > %d", pkt._data.size(), sizeof(CoprocReq_SmartServoReq_data_t::bytes));
        return;
    }

    CoprocReq req = {
        .which_payload = CoprocReq_smartServoReq_tag,
        .payload = {
            .smartServoReq = {
                .expect_response = expect_response,
                .data = {
                    .size = (pb_size_t)(pkt._data.size()),
                },
            },
        },
    };

    auto& servoReq = req.payload.smartServoReq;
    memcpy(servoReq.data.bytes, pkt._data.data(), pkt._data.size());

    if (xQueueSend(m_responseQueueQueue, &responseQueue, pdMS_TO_TICKS(1000))
        == pdFALSE) {
        ESP_LOGE(TAG,
            "Invalid use of SmartServoBusBackend, multiple requests in "
            "flight.");
        return;
    }

    Manager::get().sendToCoproc(req);
}

void SmartServoBusBackend::onCoprocStat(const CoprocStat_SmartServoStat& msg) {
    QueueHandle_t responseQueue = NULL;
    if (xQueueReceive(m_responseQueueQueue, &responseQueue, 0) == pdFALSE
        || responseQueue == NULL) {
        return;
    }

    BusBackend::rx_response resp;
    resp.size = msg.data.size;
    memcpy(resp.data, msg.data.bytes, msg.data.size);

    xQueueSend(responseQueue, &resp, 300 / portTICK_PERIOD_MS);
}
};
