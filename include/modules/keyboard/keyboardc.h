#pragma once

#include "common/module.h"
#include "common/stringmap.h"

#include <string>

namespace love::common
{
    class Keyboard : public Module
    {
      public:
        enum class KeyboardType : uint8_t;

        struct SwkbdOpt
        {
            KeyboardType type;
            bool isPassword;
            std::string hint;

            u32 maxLength;
        };

        enum KeyboardOption
        {
            OPTION_TYPE,
            OPTION_PASSCODE,
            OPTION_HINT,
            OPTION_MAX_LEN,
            OPTION_MAX_ENUM
        };

        static constexpr uint32_t DEFAULT_INPUT_LENGTH = 0x14;
        static constexpr uint32_t MINIMUM_INPUT_LENGTH = 0x01;

        Keyboard(uint32_t swkbdMaxLength);

        ~Keyboard();

        ModuleType GetModuleType() const
        {
            return M_KEYBOARD;
        }

        const char* GetName() const override
        {
            return "love.keyboard";
        }

        /* LOVE2D Functions */

        virtual std::string SetTextInput(const SwkbdOpt& options) = 0;

        /* End LÖVE Functions */

        constexpr virtual uint32_t ENCODING_MULTIPLIER() = 0;

        const uint32_t CalculateEncodingMaxLength(const uint32_t in);

        static bool GetConstant(const char* in, KeyboardOption& out);
        static bool GetConstant(KeyboardOption in, const char*& out);
        static std::vector<const char*> GetConstants(KeyboardOption);

        static const char* GetConstant(KeyboardOption in);
        static constexpr uint8_t MAX_TYPES = 3;

      protected:
        char* text;

      private:
        const static StringMap<KeyboardOption, OPTION_MAX_ENUM> keyboardOptions;
    };
} // namespace love::common
