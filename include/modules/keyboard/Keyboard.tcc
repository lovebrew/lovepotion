#pragma once

#include "common/Map.hpp"
#include "common/Module.hpp"
#include "common/Result.hpp"

#include <memory>

namespace love
{
    class KeyboardBase : public Module
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
            bool password;
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

#if defined(__SWITCH__)
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x1F4;
        static constexpr uint32_t MULTIPLIER       = 4;
#else
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x100;
        static constexpr uint32_t MULTIPLIER       = 3;
#endif

        KeyboardBase() :
            Module(M_KEYBOARD, "love.keyboard"),
            keyRepeat(false),
            showing(false),
            text(nullptr)
        {}

        virtual ~KeyboardBase()
        {}

        void setKeyRepeat(bool enable)
        {
            this->keyRepeat = enable;
        }

        bool hasKeyRepeat() const
        {
            return this->keyRepeat;
        }

        bool hasScreenKeyboard() const
        {
            return true;
        }

        bool hasTextInput() const
        {
            return this->showing;
        }

        std::string_view getText() const
        {
            return this->text.get();
        }

        // clang-format off
        STRINGMAP_DECLARE(Options, KeyboardOption,
            { "type",      OPTION_TYPE       },
            { "password",  OPTION_PASSCODE   },
            { "hint",      OPTION_HINT       },
            { "maxLength", OPTION_MAX_LENGTH }
        );

        STRINGMAP_DECLARE(KeyboardTypes, KeyboardType,
            { "normal", TYPE_NORMAL },
            { "qwerty", TYPE_QWERTY },
            { "numpad", TYPE_NUMPAD }
        );
        // clang-format on

      protected:
        virtual uint32_t getMaxEncodingLength(const uint32_t length) const
        {
            return std::clamp(length, MINIMUM_INPUT_LENGTH, MAX_INPUT_LENGTH) * MULTIPLIER;
        }

        bool keyRepeat;
        bool showing;

        std::unique_ptr<char[]> text;
    };
} // namespace love