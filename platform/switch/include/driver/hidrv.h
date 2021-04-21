#pragma once

#include "common/driver/hidrvc.h"

/*
** HID backend class for Nintendo Switch
*/

namespace love::driver
{
    class Hidrv : public common::driver::Hidrv
    {
      public:
        Hidrv();

        bool Poll(LOVE_Event* event) override;

      private:
        void CheckFocus();

        static constexpr int MAX_TOUCHES = 16;

        HidTouchScreenState touchState;

        std::array<HidTouchState, MAX_TOUCHES> stateTouches;
        std::array<HidTouchState, MAX_TOUCHES> oldStateTouches;

        size_t currentPadIndex;

        int prevTouchCount;
    };
} // namespace love::driver
