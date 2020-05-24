#pragma once

#include <atomic>

namespace rb {

/**
 * \brief Helper class for controlling the piezo.
 */
class Piezo {
    friend class Manager;

public:
    void start();
    void stop();

private:
    Piezo();
    Piezo(const Piezo&) = delete;
    ~Piezo();
};

};
