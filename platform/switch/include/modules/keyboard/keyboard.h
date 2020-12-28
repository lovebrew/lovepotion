#pragma once

#include "modules/keyboard/keyboardc.h"
#include <switch.h>

namespace love
{
    enum class common::Keyboard::KeyboardType : uint8_t
    {
        TYPE_NORMAL = SWKBD_TYPE_NORMAL,
        TYPE_QWERTY = SWKBD_TYPE_QWERTY,
        TYPE_NUMPAD = SWKBD_TYPE_NUMPAD,
        TYPE_MAX_ENUM
    };

    class Keyboard : public common::Keyboard
    {
        public:
            static constexpr uint32_t MAX_INPUT_LENGTH = 0x1F4;

            constexpr uint32_t ENCODING_MULTIPLIER() override {
                return 0x04;
            }

            Keyboard();

            std::string SetTextInput(const SwkbdOpt & options) override;

        private:
            SwkbdConfig keyboard;
    };
}