#include "common/backend/input.h"

#include "modules/touch/touch.h"
#include "modules/window/window.h"

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
        Message * message = this->Convert(event);

        if (message)
        {
            this->Push(message);
            message->Release();
        }
    }
}

love::Message * love::Event::Convert(const LOVE_Event & event)
{
    Message * message = nullptr;

    std::vector<Variant> vargs;
    vargs.reserve(4);

    const char * text = nullptr;

    Touch * touchModule = nullptr;
    Touch::TouchInfo touchinfo;

    switch (event.type)
    {
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

            touchModule = (Touch *)Module::GetInstance<Touch>(M_TOUCH);

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
        case LOVE_GAMEPADUP:
        case LOVE_GAMEPADDOWN:
        case LOVE_GAMEPADAXIS:
            message = this->ConvertJoystickEvent(event);
            break;
        case LOVE_WINDOWEVENT:
            message = this->ConvertWindowEvent(event);
            break;
        case LOVE_QUIT:
            message = new Message("quit");
            break;
        case LOVE_LOWMEMORY:
            message = new Message("lowmemory");
            break;
        default:
            break;
    }

    return message;
}

love::Message * love::Event::ConvertJoystickEvent(const LOVE_Event & event) const
{
    auto joyModule = Module::GetInstance<Joystick>(M_JOYSTICK);

    if (!joyModule)
        return nullptr;

    Message * message = nullptr;

    std::vector<Variant> vargs;
    vargs.reserve(4);

    love::Type * joystickType = &Gamepad::type;

    Gamepad * stick = nullptr;

    Gamepad::GamepadButton padButton;
    Gamepad::GamepadAxis padAxis;

    const char * text = nullptr;

    switch (event.type)
    {
        case LOVE_GAMEPADUP:
        case LOVE_GAMEPADDOWN:
        {
            if (!common::Gamepad::GetConstant(event.button.name, padButton))
                break;

            if (!common::Gamepad::GetConstant(padButton, text))
                break;

            stick = joyModule->GetJoystickFromID(event.button.which);

            if (!stick)
                break;

            vargs.emplace_back(joystickType, stick);
            vargs.emplace_back(text, strlen(text));

            message = new Message((event.type == LOVE_GAMEPADDOWN) ? "gamepadpressed" : "gamepadreleased", vargs);

            break;
        }
        case LOVE_GAMEPADAXIS:
        {
            if (!common::Gamepad::GetConstant(event.axis.axis, padAxis))
                break;

            if (!common::Gamepad::GetConstant(padAxis, text))
                break;

            stick = joyModule->GetJoystickFromID(event.axis.which);

            if (!stick)
                break;

            vargs.emplace_back(joystickType, stick);
            vargs.emplace_back(text, strlen(text));
            vargs.emplace_back(event.axis.value);

            message = new Message("gamepadaxis", vargs);

            break;
        }
        default:
            break;
    }

    return message;
}

love::Message * love::Event::ConvertWindowEvent(const LOVE_Event & event)
{
    Message * message = nullptr;

    std::vector<Variant> vargs;
    vargs.reserve(4);

    Window * windowModule = nullptr;

    if (event.type != LOVE_WINDOWEVENT)
        return nullptr;

    switch (event.subType)
    {
        case LOVE_FOCUS_LOST:
        case LOVE_FOCUS_GAINED:
        {
            vargs.emplace_back(event.type == LOVE_FOCUS_GAINED);

            message = new Message("focus", vargs);

            break;
        }
        case LOVE_RESIZE:
        {
            windowModule = Module::GetInstance<Window>(M_WINDOW);

            int width  = event.size.width;
            int height = event.size.width;

            vargs.emplace_back((float)width);
            vargs.emplace_back((float)height);

            message = new Message("resize", vargs);

            if (windowModule)
                windowModule->OnSizeChanged(width, height);

            break;
        }
        default:
            break;
    }

    return message;
}

void love::Event::ExceptionIfInRenderPass(const char * name)
{} /* potentially useless */

void love::Event::InternalClear()
{
    LOVE_Event event;

    while (Input::PollEvent(&event))
    {
        /* do nothing */
    }

    this->Clear();
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

love::Message * love::Event::Wait()
{
    LOVE_Event event;

    if (Input::PollEvent(&event) == false)
        return nullptr;

    return this->Convert(event);
}
