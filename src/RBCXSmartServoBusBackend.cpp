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

void SmartServoBusBackend::send(const lw::Packet& pkt, QueueHandle_t responseQueue,
    bool expect_response, bool priority) {

    CoprocReq req = {
        .which_payload = CoprocReq_smartServoReq_tag,
        .payload = {
            .smartServoReq = {
                .id = pkt.id(),
                .expect_response = expect_response,
                .data = {
                    .size = pkt.size() - 2,
                    .bytes = {},
                }
            }
        }
    };

    memcpy(req.payload.smartServoReq.data.bytes, pkt._data.data() + 4, pkt.size() - 2);
    if(xQueueSend(m_responseQueueQueue, &responseQueue, pdMS_TO_TICKS(1000)) == pdFALSE) {
        ESP_LOGE(TAG, "Invalid use of SmartServoBusBackend, multiple requests in flight.");
        return;
    }

    Manager::get().sendToCoproc(req);
}

void SmartServoBusBackend::onCoprocStat(const CoprocStat_SmartServoStat& msg) {
    QueueHandle_t responseQueue = NULL;
    if(xQueueReceive(m_responseQueueQueue, &responseQueue, 0) == pdFALSE || responseQueue == NULL) {
        return;
    }

    BusBackend::rx_response resp;
    resp.size = msg.data.size + 3;
    resp.data[0] = 0x55;
    resp.data[1] = 0x55;
    resp.data[2] = msg.id;
    resp.data[3] = msg.data.size + 4;
    memcpy(resp.data+4, msg.data.bytes, msg.data.size);

    xQueueSend(responseQueue, &resp, 300 / portTICK_PERIOD_MS);
}
};
