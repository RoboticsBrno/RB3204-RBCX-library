#pragma once

#include <atomic>

namespace rb {

/**
 * \brief Helper class for controlling the piezo.
 */
class Piezo {
    friend class Manager;

public:
    void start() { setState(true); } //!< Starts the piezo.
    void stop() { setState(false); } //!< Stops the piezo.

    void setState(bool on); //!< Sets the piezo state (on/off).

private:
    Piezo();
    Piezo(const Piezo&) = delete;
    ~Piezo();

    bool m_on;
};
};
