#pragma once

namespace rb {

/**
 * \brief Helper class for controlling I2C.
 */
class I2c {
    friend class Manager;

public:
    void transmit(uint16_t DevAddress, uint32_t pData, uint8_t Size);
    void receive(uint16_t DevAddress, uint8_t Size);
    void ready(uint16_t DevAddress);
    void scanner();

private:
    I2c();
    I2c(const I2c&) = delete;
    ~I2c();

};
};
