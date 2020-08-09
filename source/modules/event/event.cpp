#include "common/runtime.h"
#include "common/backend/input.h"
#include "modules/touch/touch.h"
#include "modules/event/event.h"

#if defined (_3DS)
    aptHookCookie s_aptHookCookie;
#endif


love::Event::Event()
{
    #if defined (_3DS)
        aptHook(&s_aptHookCookie, Input::CheckFocus, nullptr);
    #elif defined (__SWITCH__)
        appletLockExit();
        appletSetFocusHandlingMode(AppletFocusHandlingMode_NoSuspend);
    #endif
}

love::Event::~Event()
{
    #if defined (_3DS)
        aptUnhook(&s_aptHookCookie);
    #elif defined (__SWITCH__)
        appletSetFocusHandlingMode(AppletFocusHandlingMode_SuspendHomeSleep);
        appletUnlockExit();
    #endif
}

void love::Event::Pump()
{
    LOVE_Event event;

    while (Input::PollEvent(&event))
    {
        Message * message = nullptr;

        Gamepad * gamepad = nullptr;
        love::Type * joystickType = &Gamepad::type;

        std::vector<Variant> vargs;
        vargs.reserve(4);

        auto joystickModule = Module::GetInstance<Joystick>(Module::M_JOYSTICK);

        love::Touch * touchModule = nullptr;
        Touch::TouchInfo touchinfo;
        const char * text;

        switch (event.type)
        {
            case LOVE_GAMEPADUP:
            case LOVE_GAMEPADDOWN:
            {
                gamepad = joystickModule->GetJoystickFromID(event.button.which);

                if (!gamepad)
                    break;

                vargs.emplace_back(joystickType, gamepad);
                vargs.emplace_back(event.button.name);

                message = new Message((event.type == LOVE_GAMEPADDOWN) ?
                        "gamepadpressed" : "gamepadreleased", vargs);

                break;
            }
            case LOVE_GAMEPADAXIS:
            {
                gamepad = joystickModule->GetJoystickFromID(event.axis.which);

                if (!gamepad)
                    break;

                vargs.emplace_back(joystickType, gamepad);
                vargs.emplace_back(event.axis.axis);
                vargs.emplace_back((float)event.axis.value);

                message = new Message("gamepadaxis", vargs);

                break;
            }
            case LOVE_TOUCHPRESS:
            case LOVE_TOUCHRELEASE:
            case LOVE_TOUCHMOVED:
            {
                touchinfo.id = (int64_t)event.touch.id;
                touchinfo.x = event.touch.x;
                touchinfo.y = event.touch.y;
                touchinfo.dx = event.touch.dx;
                touchinfo.dy = event.touch.dy;
                touchinfo.pressure = event.touch.pressure;

                touchModule = (love::Touch *)Module::GetInstance<love::Touch>(M_TOUCH);

                if (touchModule)
                    touchModule->OnEvent(event.type, touchinfo);

                vargs.emplace_back((void *)(intptr_t)touchinfo.id);
                vargs.emplace_back((float)touchinfo.x);
                vargs.emplace_back((float)touchinfo.y);
                vargs.emplace_back((float)touchinfo.dx);
                vargs.emplace_back((float)touchinfo.dy);
                vargs.emplace_back((float)touchinfo.pressure);

                if (event.type == LOVE_TOUCHPRESS)
                    text = "touchpressed";
                else if (event.type == LOVE_TOUCHRELEASE)
                    text = "touchreleased";
                else
                    text = "touchmoved";

                message = new Message(text, vargs);

                break;
            }
            case LOVE_FOCUS_GAINED:
            case LOVE_FOCUS_LOST:
                vargs.emplace_back(event.type == LOVE_FOCUS_GAINED);
                message = new Message("focus", vargs);
                break;
            case LOVE_QUIT:
                message = new Message("quit");
                break;
            default:
                break;
        }

        if (message)
        {
            this->Push(message);
            message->Release();
        }
    }
}

void love::Event::Clear()
{
    thread::Lock lock(this->mutex);

    while (!this->queue.empty())
    {
        this->queue.front()->Release();
        this->queue.pop();
    }
}

bool love::Event::Poll(Message *& message)
{
    thread::Lock lock(this->mutex);

    if (this->queue.empty())
        return false;

    message = this->queue.front();
    this->queue.pop();

    return true;
};

void love::Event::Push(Message * message)
{
    thread::Lock lock(this->mutex);

    message->Retain();
    this->queue.push(message);
}
