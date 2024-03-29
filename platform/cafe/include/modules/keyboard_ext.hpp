#pragma once

#include <modules/keyboard/keyboard.tcc>

#include <objects/shader_ext.hpp>
#include <utilities/driver/framebuffer.hpp>

#include <coreinit/filesystem.h>
#include <coreinit/memdefaultheap.h>

#include <gx2/context.h>
#include <nn/swkbd.h>

namespace love
{
    template<>
    class Keyboard<Console::CAFE> : public Keyboard<Console::ALL>
    {
      public:
        static constexpr uint32_t MAX_INPUT_LENGTH = 0x100;

        Keyboard();

        void Initialize();

        virtual ~Keyboard();

        void SetTextInput(const KeyboardOptions& options);

        const uint32_t GetMaxEncodingLength(const uint32_t in)
        {
            return in * 0x04;
        }

        const nn::swkbd::State GetState() const
        {
            return nn::swkbd::GetStateInputForm();
        }

        const bool HasTextInput() const
        {
            return this->IsShowing();
        }

        const bool IsShowing() const
        {
            return this->GetState() != nn::swkbd::State::Hidden;
        }

        const bool IsHiding() const
        {
            return this->GetState() == nn::swkbd::State::FadeOut;
        }

        void HideKeyboard()
        {
            nn::swkbd::DisappearInputForm();
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
        GX2ContextState* state;

        nn::swkbd::CreateArg createArgs;
        nn::swkbd::AppearArg appearArgs;
        FSClient* client;

        bool inited;
    };
} // namespace love
