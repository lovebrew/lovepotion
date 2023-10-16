#pragma once

#include <modules/keyboard/keyboard.tcc>
#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <switch.h>

namespace love
{
    template<>
    class Keyboard<Console::HAC> : public Keyboard<Console::ALL>
    {
      public:
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x1F4;

        const uint32_t GetMaxEncodingLength(const uint32_t in)
        {
            return in * 0x04;
        }

        Keyboard();

        void SetTextInput(const KeyboardOptions& options);

        const bool HasTextInput() const
        {
            return this->showing;
        }

        // clang-format off
        static constexpr BidirectionalMap keyboardTypes = {
            "normal", SwkbdType_Normal,
            "qwerty", SwkbdType_QWERTY,
            "numpad", SwkbdType_NumPad
        };
        // clang-format on

      private:
        SwkbdConfig config;
        bool showing;
    };
} // namespace love
