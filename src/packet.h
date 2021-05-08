#pragma once

#include <vector>
#include <stdint.h>
#include <cstddef>
#include <esp_log.h>

class Packet {
public:
    Packet(uint8_t command = 0, uint8_t device_id = 1) : m_ritr(index_data) {
        m_data.push_back(0xFF);
        m_data.push_back(device_id);
        m_data.push_back(command);
        m_data.push_back(0); // length placeholder
    }

    void reset(uint8_t command = 0, uint8_t device_id = 1) {
        m_data.resize(index_data);
        m_data[index_device_id] = device_id;
        m_data[index_command] = command;
        m_data[index_length] = 0;

        m_ritr = index_data;
    }

    template<typename T>
    void write(T val) {
        const size_t cur_size = m_data.size();
        if(cur_size + sizeof(T) > 255) {
            ESP_LOGE(TAG, "invalid write() with index %d and size %d", cur_size, sizeof(T));
            return;
        }

        m_data.resize(cur_size + sizeof(T));
        memcpy(m_data.data() + cur_size, &val, sizeof(T));
        m_data[index_length] += sizeof(T);
    }

    template<typename T>
    T read() const {
        T val{};
        if(m_ritr + sizeof(T) <= m_data.size()) {
            memcpy(&val, m_data.data() + m_ritr, sizeof(T));
            m_ritr += sizeof(T);
        } else {
            ESP_LOGE(TAG, "invalid read() with index %d and size %d", m_ritr, sizeof(T));
        }
        return val;
    }

    template<typename T>
    T read(size_t idx) const {
        T val{};
        if(idx + sizeof(T) <= m_data.size()) {
            memcpy(&val, m_data.data() + idx, sizeof(T));
        } else {
            ESP_LOGE(TAG, "invalid read() with index %d and size %d", idx, sizeof(T));
        }
        return val;
    }

    uint8_t command() const {
        return m_data[index_command];
    }

    uint8_t deviceId() const {
        return m_data[index_device_id];
    }

    uint8_t data_length() const {
        return m_data[index_length];
    }

    const std::vector<uint8_t>& raw() const { return m_data; }

private:
    static constexpr size_t index_device_id = 1;
    static constexpr size_t index_command = 2;
    static constexpr size_t index_length = 3;
    static constexpr size_t index_data = 4;

    static constexpr const char *TAG = "Packet";

    std::vector<uint8_t> m_data;
    mutable size_t m_ritr;
};

class PacketParser {
public:
    PacketParser() : m_state(st_start) { }

    bool addByte(uint8_t b) {
        switch(m_state) {
        case st_start:
            if(b != 0xFF)
                return false;
            break;
        case st_device:
            m_device = b;
            break;
        case st_command:
            m_command = b;
            break;
        case st_length:
            m_length = b;
            m_packet.reset(m_command, m_device);
            break;
        case st_data:
            m_packet.write(b);
            if(m_packet.data_length() < m_length)
                return false;
            m_state = st_start;
            return true;
        }

        ++m_state;
        return false;
    }

    const Packet& packet() const { return m_packet; }

private:
    enum  {
        st_start,
        st_device,
        st_command,
        st_length,
        st_data
    };

    int m_state;
    uint8_t m_device;
    uint8_t m_command;
    uint8_t m_length;
    Packet m_packet;
};
