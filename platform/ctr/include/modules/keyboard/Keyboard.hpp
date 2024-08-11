#pragma once

#include "modules/keyboard/Keyboard.tcc"

#include <3ds.h>

namespace love
{
    class Keyboard : public KeyboardBase
    {
      public:
        Keyboard();

        virtual ~Keyboard()
        {}

        void setTextInput(const KeyboardOptions& options);

        using KeyboardBase::getConstant;

        // clang-format off
        ENUMMAP_DECLARE(CtrKeyboardTypes, KeyboardType, SwkbdType,
            { TYPE_NORMAL, SWKBD_TYPE_NORMAL },
            { TYPE_QWERTY, SWKBD_TYPE_QWERTY },
            { TYPE_NUMPAD, SWKBD_TYPE_NUMPAD }
        );

        ENUMMAP_DECLARE(CtrKeyboardResults, KeyboardResult, SwkbdCallbackResult,
            { RESULT_OK,       SWKBD_CALLBACK_OK       },
            { RESULT_CANCEL,   SWKBD_CALLBACK_CLOSE    },
            { RESULT_CONTINUE, SWKBD_CALLBACK_CONTINUE }
        );
        // clang-format on

      private:
        SwkbdState state;
    };
} // namespace love
