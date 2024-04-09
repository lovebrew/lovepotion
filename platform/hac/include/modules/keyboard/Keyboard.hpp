#pragma once

#include "modules/keyboard/Keyboard.tcc"

#include <switch.h>

namespace love
{
    class Keyboard : public KeyboardBase
    {
      public:
        Keyboard();

        virtual ~Keyboard()
        {}

        void setTextInput(const KeyboardOptions& options);

        using KeyboardBase::getConstant;

        // clang-format off
        ENUMMAP_DECLARE(HacKeyboardTypes,  KeyboardType, SwkbdType,
            { TYPE_NORMAL, SwkbdType_Normal },
            { TYPE_NUMPAD, SwkbdType_NumPad },
            { TYPE_QWERTY, SwkbdType_QWERTY }
        );
        // clang-format on

      private:
        SwkbdConfig config;
    };
} // namespace love
