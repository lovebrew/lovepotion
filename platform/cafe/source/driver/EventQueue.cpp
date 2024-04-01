#include "driver/EventQueue.hpp"

#include "modules/joystick/JoystickModule.hpp"

using namespace love;

#define JOYSTICK_MODULE() Module::getInstance<JoystickModule>(Module::M_JOYSTICK)

namespace love
{
    EventQueue::EventQueue()
    {}

    EventQueue::~EventQueue()
    {}

    void EventQueue::pollInternal()
    {
        if (!JOYSTICK_MODULE())
            return;

        auto& status   = ((Joystick*)JOYSTICK_MODULE()->getJoystickFromID(0))->getVPADStatus();
        auto touchType = SUBTYPE_TOUCHPRESS;

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

        for (size_t index = 0; index < JOYSTICK_MODULE()->getJoystickCount(); index++)
        {
            auto* joystick = JOYSTICK_MODULE()->getJoystick(index);

            if (joystick == nullptr)
                continue;

            joystick->update();
        }
    }
} // namespace love
