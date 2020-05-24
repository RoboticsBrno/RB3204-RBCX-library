#pragma once

namespace rb {

/**
 * \brief Helper class for controlling the LEDs connected to the expander.
 */
class Leds {
    friend class Manager;

public:
    void red(bool on = true); //!< Set the red LED state
    void yellow(bool on = true); //!< Set the yellow LED state
    void green(bool on = true); //!< Set the green LED state
    void blue(bool on = true); //!< Set the blue LED state

private:
    Leds();
    Leds(const Leds&) = delete;
    ~Leds();
};

};
