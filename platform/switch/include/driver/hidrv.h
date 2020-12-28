#pragma once

#include "common/driver/hidrvc.h"

namespace love::driver
{
    class Hidrv : public common::driver::Hidrv
    {
        public:
            Hidrv();

            bool Poll(LOVE_Event * event) override;

            bool IsDown(size_t button) override;

        private:
            void CheckFocus();

            static constexpr int MAX_TOUCHES = 16;

            HidAnalogStickState sticks[2];
            HidAnalogStickState oldSticks[2];

            HidTouchScreenState touchState;

            std::array<HidTouchState, MAX_TOUCHES> stateTouches;
            std::array<HidTouchState, MAX_TOUCHES> oldStateTouches;

            PadState currentPad;
            size_t currentPadIndex;

            int prevTouchCount;
    };

    inline driver::Hidrv hidrv;
}