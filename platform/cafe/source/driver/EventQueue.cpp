#include "driver/EventQueue.hpp"

#include "modules/joystick/JoystickModule.hpp"
#include "utility/guid.hpp"

using namespace love;

#define JOYSTICK_MODULE() Module::getInstance<JoystickModule>(Module::M_JOYSTICK)

namespace love
{
    EventQueue::EventQueue() : gamepad(nullptr), previousTouch {}
    {}

    EventQueue::~EventQueue()
    {}

    void EventQueue::pollInternal()
    {
        if (!JOYSTICK_MODULE())
            return;

        for (int index = 0; index < JOYSTICK_MODULE()->getJoystickCount(); index++)
        {
            auto* joystick = JOYSTICK_MODULE()->getJoystick(index);

            if (joystick == nullptr)
                continue;

            if (joystick->getGamepadType() == GAMEPAD_TYPE_NINTENDO_WII_U_GAMEPAD)
                this->gamepad = (Joystick*)joystick;

            JOYSTICK_MODULE()->getJoystick(index)->update();
        }

        if (this->gamepad)
        {
            auto& status = this->gamepad->getVPADStatus();
            SubEventType touchType;

            if (status.tpNormal.touched)
            {
                // clang-format off
                VPADGetTPCalibratedPointEx(VPAD_CHAN_0, VPAD_TP_854X480, &status.tpNormal, &status.tpNormal);
                // clang-format on

                float x = status.tpNormal.x, y = status.tpNormal.y;
                float dx = 0, dy = 0;

                dx = (status.tpNormal.x - this->previousTouch.x);
                dy = (status.tpNormal.y - this->previousTouch.y);

                if (dx == 0 && dy == 0)
                    touchType = SUBTYPE_TOUCHPRESS;
                else
                    touchType = SUBTYPE_TOUCHMOVED;

                this->sendTouchEvent(touchType, 0, x, y, dx, dy, 1.0f);
                this->previousTouch = status.tpNormal;

                if (touchType == SUBTYPE_TOUCHMOVED && !dx && !dy)
                    this->events.pop_back();
            }
            else
            {
                const auto& previous = this->previousTouch;
                this->sendTouchEvent(SUBTYPE_TOUCHRELEASE, 0, previous.x, previous.y, 0, 0, 1.0f);
            }
        }
    }
} // namespace love
