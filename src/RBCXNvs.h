#pragma once

#include <string>

#include <esp_system.h>
#include <nvs.h>
#include <nvs_flash.h>

namespace rb {

class Nvs {
public:
    Nvs(const char* name_space, const char* partition = "nvs");
    ~Nvs();
    Nvs(const Nvs&) = delete;

    bool existsInt(const char* key); //!< Returns true if the key for the int exists in the NVS
    int getInt(const char* key); //!< Returns the int value of the key in the NVS
    void writeInt(const char* key, int value); //!< Writes the int value to the key in the NVS

    bool existsString(const char* key); //!< Returns true if the key for the string exists in the NVS
    std::string getString(const char* key); //!< Returns the string value of the key in the NVS
    void writeString(const char* key, const std::string& value); //!< Writes the string value to the key in the NVS

    void commit(); //!< Commits the changes to the NVS (it nessecary to call this to save the changes)

private:
    esp_err_t initFlash();

    nvs_handle m_handle;
    bool m_dirty;
};

} // namespace rb
