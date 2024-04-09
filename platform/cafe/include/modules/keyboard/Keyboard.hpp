#pragma once

#include "modules/keyboard/Keyboard.tcc"

#include <coreinit/filesystem.h>
#include <coreinit/memdefaultheap.h>

#include <gx2/context.h>
#include <nn/swkbd.h>

namespace love
{
    class Keyboard : public KeyboardBase
    {
      public:
        Keyboard();

        ~Keyboard();

        void initialize();

        void hide()
        {
            this->showing = false;
            nn::swkbd::DisappearInputForm();
        }

        void setTextInput(const KeyboardOptions& options);

        using KeyboardBase::getConstant;

        // clang-format off
        ENUMMAP_DECLARE(CafeKeyboardType, KeyboardType, nn::swkbd::KeyboardMode,
            { TYPE_NORMAL, nn::swkbd::KeyboardMode::Full   },
            { TYPE_QWERTY, nn::swkbd::KeyboardMode::Utf8   },
            { TYPE_NUMPAD, nn::swkbd::KeyboardMode::Numpad }
        );
        // clang-format on

      private:
        nn::swkbd::CreateArg createArgs;
        nn::swkbd::AppearArg appearArgs;
        FSClient* client;

        bool inited;
    };
} // namespace love
