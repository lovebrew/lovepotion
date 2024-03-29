#pragma once

#include <modules/keyboard/keyboard.tcc>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <3ds.h>

namespace love
{
    template<>
    class Keyboard<Console::CTR> : public Keyboard<Console::ALL>
    {
      public:
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x100;

        Keyboard();

        void SetTextInput(const KeyboardOptions& options);

        const uint32_t GetMaxEncodingLength(const uint32_t in)
        {
            return in * 0x03;
        }

        const bool HasTextInput() const
        {
            return this->showing;
        }

        // clang-format off
        static constexpr BidirectionalMap keyboardTypes = {
            "normal", SWKBD_TYPE_NORMAL,
            "qwerty", SWKBD_TYPE_QWERTY,
            "numpad", SWKBD_TYPE_NUMPAD
        };
        // clang-format on

      private:
        SwkbdState state;
        bool showing;
    };
} // namespace love
