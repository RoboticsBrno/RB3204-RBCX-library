#include "RBCXManager.h"
#include "RBCXOled.h"

#include "rbcx.pb.h"


namespace rb {

Oled::Oled()
    : m_init(false) 
    , m_width(128)
    {}

Oled::~Oled() {}

void Oled::init(OledType type, bool rotate, bool inverseColor) {
    switch (type)
    {
        case Oled_128x32:
            m_height = 32;
            break;    

        case Oled_128x64:
            m_height = 64;
            break;
    }

    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_init_tag,
                .oledCmd = {
                    .init = {
                        .height = m_height,
                        .width = 128,
                        .rotate = rotate,
                        .inverseColor = inverseColor,                           
                    },
                }
            }
        }
    });   
}

void Oled::fill(OledColor color) {
        rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_fill_tag,
                .oledCmd = {
                    .fill = CoprocReq_OledColor(color),
                }
            }
        }
    }); 
}

void Oled::updateScreen(void) {
    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_update_tag,
            }
        }
    }); 
}
void Oled::drawPixel(uint8_t x, uint8_t y, OledColor color) {
    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_drawPixel_tag,
                .oledCmd = {
                    .drawPixel = {
                        .x = x,
                        .y = y,
                        .color = CoprocReq_OledColor(color),
                    }
                }
            }
        }
    });
}

void Oled::writeString(const char* str, OledFontDef Font, OledColor color) {
    auto message = CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_writeString_tag,
            }
        }
    };
    message.payload.oledReq.oledCmd.writeString.font = CoprocReq_OledFont(Font);
    message.payload.oledReq.oledCmd.writeString.color = CoprocReq_OledColor(color); 
    strcpy(message.payload.oledReq.oledCmd.writeString.text, str);
    rb::Manager::get().sendToCoproc(message);
}

void Oled::writeString(char* str, OledFontDef Font, OledColor color) {
    auto message = CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_writeString_tag,
            }
        }
    };
    message.payload.oledReq.oledCmd.writeString.font = CoprocReq_OledFont(Font);
    message.payload.oledReq.oledCmd.writeString.color = CoprocReq_OledColor(color); 
    strcpy(message.payload.oledReq.oledCmd.writeString.text, str);
    rb::Manager::get().sendToCoproc(message);
}

void Oled::writeString(String str, OledFontDef Font, OledColor color) {
    auto message = CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_writeString_tag,
                .oledCmd = {
                    .writeString = {
                        .text = {},
                        .font = CoprocReq_OledFont(Font),
                        .color = CoprocReq_OledColor(color),
                    }
                }
            }
        }
    };
    // message.payload.oledReq.oledCmd.writeString.font = CoprocReq_OledFont(Font);
    // message.payload.oledReq.oledCmd.writeString.color = CoprocReq_OledColor(color);
    strcpy(message.payload.oledReq.oledCmd.writeString.text, str.c_str());
    rb::Manager::get().sendToCoproc(message);
}

void Oled::setCursor(uint8_t x, uint8_t y) {
    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_setCursor_tag,
                .oledCmd = {
                    .setCursor = {
                        .x = x,
                        .y = y,
                    }
                }
            }
        }
    });
}

void Oled::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OledColor color) {
    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_drawLine_tag,
                .oledCmd = {
                    .drawLine = {
                        .x1 = x1,
                        .y1 = y1,
                        .x2 = x2,
                        .y2 = y2,
                        .color = CoprocReq_OledColor(color),
                    }
                }
            }
        }
    });
}

void Oled::drawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, OledColor color) {
    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_drawArc_tag,
                .oledCmd = {
                    .drawArc = {
                        .x = x,
                        .y = y,
                        .radius = radius,
                        .start_angle = start_angle,
                        .sweep = sweep,
                        .color = CoprocReq_OledColor(color),
                    }
                }
            }
        }
    });
}

void Oled::drawCircle(uint8_t x, uint8_t y, uint8_t radius, OledColor color) {
    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_drawCircle_tag,
                .oledCmd = {
                    .drawCircle = {
                        .x = x,
                        .y = y,
                        .radius = radius,
                        .color = CoprocReq_OledColor(color),
                    }
                }
            }
        }
    });
}

void Oled::drawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OledColor color) {
    rb::Manager::get().sendToCoproc(CoprocReq { 
        .which_payload = CoprocReq_oledReq_tag, 
        .payload = {
            .oledReq = {
                .which_oledCmd = CoprocReq_OledReq_drawRectangle_tag,
                .oledCmd = {
                    .drawLine = {
                        .x1 = x1,
                        .y1 = y1,
                        .x2 = x2,
                        .y2 = y2,
                        .color = CoprocReq_OledColor(color),
                    }
                }
            }
        }
    });
}


}; //namespace rb
