#include "common/screen.hpp"

#include "driver/EventQueue.hpp"

#include "modules/joystick/JoystickModule.hpp"

#define JOYSTICK_MODULE() love::Module::getInstance<love::JoystickModule>(love::Module::M_JOYSTICK)

namespace love
{
    EventQueue::EventQueue() :
        EventQueueBase<EventQueue>(),
        padStyleUpdates {},
        previousTouchCount(0),
        touches {},
        oldTouches {},
        initAdded(false)
    {
        for (int index = 0; index < 0x08; index++)
        {
            auto player = HidNpadIdType(HidNpadIdType_No1 + index);
            hidAcquireNpadStyleSetUpdateEventHandle(player, &this->padStyleUpdates[index], true);
        }
    }

    EventQueue::~EventQueue()
    {
        for (int index = 0; index < 0x08; index++)
            eventClose(&this->padStyleUpdates[index]);
    }

    static void checkFocus()
    {
        auto focused = (appletGetFocusState() == AppletFocusState_InFocus);

        uint32_t message  = 0;
        const auto result = appletGetMessage(&message);

        if (R_SUCCEEDED(result))
        {
            bool shouldClose = !appletProcessMessage(message);

            if (shouldClose)
            {
                EventQueue::getInstance().sendQuit();
                return;
            }

            switch (message)
            {
                case AppletMessage_FocusStateChanged:
                {
                    bool oldFocused             = focused;
                    AppletFocusState focusState = appletGetFocusState();

                    focused = (focusState == AppletFocusState_InFocus);
                    EventQueue::getInstance().sendFocus(focused);

                    if (focused == oldFocused)
                        break;

                    if (focused)
                        appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
                    else
                        appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleepNotify);

                    break;
                }
                case AppletMessage_OperationModeChanged:
                {
                    auto info = love::getScreenInfo(Screen(0));
                    EventQueue::getInstance().sendResize(info.width, info.height);

                    break;
                }
                default:
                    break;
            }
        }
    }

    void EventQueue::pollInternal()
    {
        checkFocus();

        hidGetTouchScreenStates(&this->touchState, 1);
        const auto touchCount = this->touchState.count;

        if (touchCount > 0)
        {
            for (int id = 0; id < touchCount; id++)
            {
                auto touchType = SUBTYPE_TOUCHPRESS;

                if (touchCount > this->previousTouchCount && id >= this->previousTouchCount)
                {
                    this->touches[id]    = this->touchState.touches[id];
                    this->oldTouches[id] = this->touches[id];

                    touchType = SUBTYPE_TOUCHPRESS;
                }
                else
                {
                    this->oldTouches[id] = this->touches[id];
                    this->touches[id]    = this->touchState.touches[id];

                    touchType = SUBTYPE_TOUCHMOVED;
                }

                float x = this->touches[id].x, y = this->touches[id].y;

                int32_t dx = this->touches[id].x - this->oldTouches[id].x;
                int32_t dy = (int32_t)this->touches[id].y - this->oldTouches[id].y;

                this->sendTouchEvent(touchType, id, x, y, dx, dy, 1.0f);

                if (touchType == SUBTYPE_TOUCHMOVED && !dx && !dy)
                {
                    this->events.pop_back();
                    continue;
                }
            }
        }

        if (touchCount < this->previousTouchCount)
        {
            for (int id = 0; id < this->previousTouchCount; ++id)
            {
                float x = this->touches[id].x, y = this->touches[id].y;
                this->sendTouchEvent(SUBTYPE_TOUCHRELEASE, id, x, y, 0.0f, 0.0f, 0.0f);
            }
        }

        this->previousTouchCount = touchCount;

        if (!JOYSTICK_MODULE())
            return;

        int current = JOYSTICK_MODULE()->getJoystickCount();

        for (int index = 0; index < 0x08; index++)
        {
            if (R_SUCCEEDED(eventWait(&this->padStyleUpdates[index], 0)))
            {
                int newCount = JOYSTICK_MODULE()->getJoystickCount();

                if (newCount != current)
                    this->sendJoystickStatus(newCount > current, index);

                if (!initAdded)
                {
                    this->sendJoystickStatus(true, index);
                    initAdded = true;
                }
            }
        }

        for (int index = 0; index < current; index++)
        {
            auto* joystick = JOYSTICK_MODULE()->getJoystick(index);

            if (joystick == nullptr)
                continue;

            const auto id = joystick->getID();
            joystick->update();

            for (int input = 0; input < JoystickBase::GAMEPAD_BUTTON_MAX_ENUM; input++)
            {
                std::vector<JoystickBase::GamepadButton> inputs = { JoystickBase::GamepadButton(input) };

                if (joystick->isDown(inputs))
                    this->sendGamepadButtonEvent(SUBTYPE_GAMEPADDOWN, id, input);

                if (joystick->isUp(inputs))
                    this->sendGamepadButtonEvent(SUBTYPE_GAMEPADUP, id, input);
            }

            for (int input = 0; input < JoystickBase::GAMEPAD_AXIS_MAX_ENUM; input++)
            {
                if (joystick->isAxisChanged(JoystickBase::GamepadAxis(input)))
                {
                    float value = joystick->getAxis(JoystickBase::GamepadAxis(input));
                    this->sendGamepadAxisEvent(id, input, value);
                }
            }

            for (int input = 0; input < Sensor::SENSOR_MAX_ENUM; input++)
            {
                if (!joystick->isSensorEnabled(Sensor::SensorType(input)))
                    continue;

                auto data = joystick->getSensorData(Sensor::SensorType(input));
                this->sendGamepadSensorEvent(id, input, data);
            }
        }
    }
} // namespace love
