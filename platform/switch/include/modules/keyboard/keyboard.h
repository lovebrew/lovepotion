#pragma once

#include "modules/keyboard/keyboardc.h"
#include <switch.h>

namespace love
{
    enum class common::Keyboard::KeyboardType : uint8_t
    {
        TYPE_NORMAL = SwkbdType_Normal,
        TYPE_QWERTY = SwkbdType_QWERTY,
        TYPE_NUMPAD = SwkbdType_NumPad
    };

    class Keyboard : public common::Keyboard
    {
      public:
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x1F4;

        constexpr uint32_t ENCODING_MULTIPLIER() override
        {
            return 0x04;
        }

        Keyboard();

        virtual ~Keyboard()
        {}

        std::string SetTextInput(const SwkbdOpt& options) override;

        static bool GetConstant(const char* in, KeyboardType& out);
        static bool GetConstant(KeyboardType in, const char*& out);
        static std::vector<const char*> GetConstants(KeyboardType);

      private:
        SwkbdConfig keyboard;
    };
} // namespace love
