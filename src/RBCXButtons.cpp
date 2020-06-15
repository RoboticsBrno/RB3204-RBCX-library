#include "RBCXButtons.h"

namespace rb {

Buttons::Buttons()
    : m_buttonsSet {} {}

Buttons::~Buttons() {}

void Buttons::setState(const CoprocStat_ButtonsStat& msg) {
    m_buttonsSet = (ButtonId)msg.buttonsPressed;
}

bool Buttons::byId(ButtonId id) const { return (m_buttonsSet & id) != 0; }
};
