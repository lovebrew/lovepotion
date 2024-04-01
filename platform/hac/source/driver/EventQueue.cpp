#include "driver/EventQueue.hpp"

#include "modules/joystick/JoystickModule.hpp"

#define JOYSTICK_MODULE() love::Module::getInstance<love::JoystickModule>(love::Module::M_JOYSTICK)

namespace love
{
    EventQueue::EventQueue() : EventQueueBase<EventQueue>()
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
                }
            }
        }
    }

    void EventQueue::pollInternal()
    {
        checkFocus();

        if (!JOYSTICK_MODULE())
            return;

        int current = JOYSTICK_MODULE()->getJoystickCount();

        // for (int index = 0; index < 0x08; index++)
        // {
        //     if (R_SUCCEEDED(eventWait(&this->padStyleUpdates[index], 0)))
        //     {
        //         int newCount = JOYSTICK_MODULE()->getJoystickCount();

        //         if (newCount != current)
        //             this->sendJoystickStatus(newCount > current, index);
        //     }
        // }

        for (int index = 0; index < current; index++)
        {
            auto* joystick = JOYSTICK_MODULE()->getJoystick(index);

            if (joystick == nullptr)
                continue;

            joystick->update();
        }
    }
} // namespace love