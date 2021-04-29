#include <esp_log.h>

#include "RBCXManager.h"
#include "RBCXI2c.h"

#include "rbcx.pb.h"

namespace rb {

I2c::I2c() {}

I2c::~I2c() {}

void I2c::transmit(uint16_t DevAddress, uint32_t pData, uint8_t Size) {
    Manager::get().sendToCoproc(CoprocReq {
        .which_payload = CoprocReq_i2cReq_tag,
        .payload = {
            .i2cReq = {
                .which_i2cCmd = CoprocReq_I2cReq_transmit_tag,
                .i2cCmd = {
                    .transmit = {
                        .devAdress = DevAddress,
                        .data = pData,
                        .size = Size,
                    },
                }
            }
        }
    });
}

void I2c::receive(uint16_t DevAddress, uint8_t Size) {
    static uint32_t id = 0;
    Manager::get().sendToCoproc(CoprocReq {
        .which_payload = CoprocReq_i2cReq_tag,
        .payload = {
            .i2cReq = {
                .which_i2cCmd = CoprocReq_I2cReq_receive_tag,
                .i2cCmd = {
                    .receive = {
                        .devAdress = DevAddress,
                        .data = id,
                        .size = Size,
                    },
                }
            }
        }
    });
}

void I2c::ready(uint16_t DevAddress) {
    Manager::get().sendToCoproc(CoprocReq {
        .which_payload = CoprocReq_i2cReq_tag,
        .payload = {
            .i2cReq = {
                .which_i2cCmd = CoprocReq_I2cReq_ready_tag,
                .i2cCmd = {
                    .ready = DevAddress,
                }
            }
        }
    });
}

void I2c::scanner(){
    Manager::get().sendToCoproc(CoprocReq {
        .which_payload = CoprocReq_i2cReq_tag,
        .payload = {
            .i2cReq = {
                .which_i2cCmd = CoprocReq_I2cReq_scan_tag,
            }
        }
    });
}

};