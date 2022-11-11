#pragma once

#include <modules/keyboard/keyboard.tcc>

#include <coreinit/filesystem.h>
#include <coreinit/memdefaultheap.h>
#include <nn/swkbd.h>

namespace love
{
    template<>
    class Keyboard<Console::CAFE> : public Keyboard<Console::ALL>
    {
      public:
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x100;

        Keyboard();

        virtual ~Keyboard();

        void SetTextInput(const KeyboardOptions& options);

        const uint32_t GetMaxEncodingLength(const uint32_t in)
        {
            return in * 0x03;
        }

        const bool IsShowing() const
        {
            return this->showing;
        }

        void HideKeyboard()
        {
            this->showing = false;
        }

        void Utf16toUtf8Text();

        // clang-format off
        static constexpr BidirectionalMap keyboardTypes = {
            "normal", (uint8_t)nn::swkbd::KeyboardMode::Full,
            "qwerty", (uint8_t)nn::swkbd::KeyboardMode::Utf8,
            "numpad", (uint8_t)nn::swkbd::KeyboardMode::Numpad
        };
        // clang-format on

      private:
        nn::swkbd::CreateArg createArgs;
        nn::swkbd::AppearArg appearArgs;
        FSClient* client;

        bool showing;
    };
} // namespace love
