#pragma once

#include "modules/keyboard/keyboardc.h"

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
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x100;

        constexpr uint32_t ENCODING_MULTIPLIER() override
        {
            return 0x03;
        }

        Keyboard();

        virtual ~Keyboard()
        {}

        std::string SetTextInput(const SwkbdOpt& options) override;

        static bool GetConstant(const char* in, KeyboardType& out);
        static bool GetConstant(KeyboardType in, const char*& out);
        static std::vector<std::string> GetConstants(KeyboardType);

      private:
        SwkbdState keyboard;

        static StringMap<KeyboardType, uint8_t(KeyboardType::TYPE_MAX_ENUM)>::Entry
            keyboardTypeEntries[];
        static StringMap<KeyboardType, uint8_t(KeyboardType::TYPE_MAX_ENUM)> keyboardTypes;
    };
} // namespace love