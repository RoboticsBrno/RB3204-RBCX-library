#pragma once

#include <functional>
#include <mutex>
#include <vector>

#include "RBCXUtil.h"

#include "rbcx.pb.h"

// Arduino defines B1...
#ifdef B1
#undef B1
#endif

namespace rb {

/**
 * \brief Helper class for controlling the LEDs connected to the expander.
 */

enum ButtonId : uint32_t {
    Off = CoprocStat_ButtonsEnum_BOFF,
    On = CoprocStat_ButtonsEnum_BON,

    B1 = CoprocStat_ButtonsEnum_B1,
    B2 = CoprocStat_ButtonsEnum_B2,
    B3 = CoprocStat_ButtonsEnum_B3,
    B4 = CoprocStat_ButtonsEnum_B4,

    Up = B1,
    Down = B2,
    Left = B3,
    Right = B4,
};

RBCX_ENUM_IMPL_MASK_OPERATORS(ButtonId)

class Buttons {
    friend class Manager;

public:
    typedef std::function<bool(ButtonId, bool)> callback_t;

    static constexpr uint32_t Count = 6;


    inline bool byId(ButtonId id) const { return (m_buttonsSet & id) != 0; }; //!< Returns true if the button `ButtonId` is pressed.
    inline bool up() const { return byId(ButtonId::Up); } //!< Returns true if the up button is pressed.
    inline bool down() const { return byId(ButtonId::Down); } //!< Returns true if the down button is pressed.
    inline bool left() const { return byId(ButtonId::Left); } //!< Returns true if the left button is pressed.
    inline bool right() const { return byId(ButtonId::Right); } //!< Returns true if the right button is pressed.
    inline bool on() const { return byId(ButtonId::On); } //!< Returns true if the on button is pressed.
    inline bool off() const { return byId(ButtonId::Off); } //!< Returns true if the off button is pressed.

    /**
     * @brief Register a callback that will be called when any button changes state.
     * The callback will be called with the button ID and the new state.
     * @param callback
     */
    void onChange(callback_t callback);

private:
    Buttons();
    Buttons(const Buttons&) = delete;
    ~Buttons();

    void setState(const CoprocStat_ButtonsStat& msg);

    std::vector<callback_t> m_callbacks;
    std::recursive_mutex m_mutex;
    ButtonId m_buttonsSet;
};
};
