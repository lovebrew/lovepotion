#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <memory>
#include <stdint.h>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Keyboard : public Module
    {
      public:
        enum KeyboardType
        {
            TYPE_NORMAL,
            TYPE_QWERTY,
            TYPE_NUMPAD
        };

        struct KeyboardOptions
        {
            uint8_t type;
            bool isPassword;
            std::string_view hint;
            uint32_t maxLength;
        };

        enum KeyboardOption
        {
            OPTION_TYPE,
            OPTION_PASSCODE,
            OPTION_HINT,
            OPTION_MAX_LENGTH,
            OPTION_MAX_ENUM
        };

        static constexpr uint32_t DEFAULT_INPUT_LENGTH = 0x14;
        static constexpr uint32_t MINIMUM_INPUT_LENGTH = 0x01;

        Keyboard(uint32_t maxTextLength)
        {
            try
            {
                this->text = std::make_unique<char[]>(maxTextLength + 1);
            }
            catch (const std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }
        }

        ModuleType GetModuleType() const override
        {
            return M_KEYBOARD;
        }

        const char* GetName() const override
        {
            return "love.keyboard";
        }

        const bool HasTextInput() const
        {
            return true;
        }

        const bool HasScreenKeyboard() const
        {
            return true;
        }

        std::string_view GetText() const
        {
            return this->text.get();
        }

        // clang-format off
        static constexpr BidirectionalMap keyboardOptions = {
            "type",      KeyboardOption::OPTION_TYPE,
            "password",  KeyboardOption::OPTION_PASSCODE,
            "hint",      KeyboardOption::OPTION_HINT,
            "length",    KeyboardOption::OPTION_MAX_LENGTH
        };
        // clang-format on

      protected:
        std::unique_ptr<char[]> text;
    };
} // namespace love
