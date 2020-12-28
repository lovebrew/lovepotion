#include "modules/touch/touch.h"
#include "modules/window/window.h"

#include "modules/event/eventc.h"

using namespace love::driver;

void love::common::Event::Pump()
{
    Hidrv::LOVE_Event event;

    while (hidrv.Poll(&event))
    {
        Message * message = this->Convert(event);

        if (message)
        {
            this->Push(message);
            message->Release();
        }
    }
}

love::Message * love::common::Event::Convert(const Hidrv::LOVE_Event & event)
{
    Message * message = nullptr;

    std::vector<Variant> vargs;
    vargs.reserve(4);

    const char * text = nullptr;

    Touch * touchModule = nullptr;
    Touch::TouchInfo touchinfo;

    switch (event.type)
    {
        case Hidrv::TYPE_TOUCHPRESS:
        case Hidrv::TYPE_TOUCHRELEASE:
        case Hidrv::TYPE_TOUCHMOVED:
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

            if (event.type == Hidrv::TYPE_TOUCHPRESS)
                text = "touchpressed";
            else if (event.type == Hidrv::TYPE_TOUCHRELEASE)
                text = "touchreleased";
            else
                text = "touchmoved";

            message = new Message(text, vargs);

            break;
        }
        case Hidrv::TYPE_GAMEPADUP:
        case Hidrv::TYPE_GAMEPADDOWN:
        case Hidrv::TYPE_GAMEPADAXIS:
            message = this->ConvertJoystickEvent(event);
            break;
        case Hidrv::TYPE_WINDOWEVENT:
            message = this->ConvertWindowEvent(event);
            break;
        case Hidrv::TYPE_QUIT:
            message = new Message("quit");
            break;
        case Hidrv::TYPE_LOWMEMORY:
            message = new Message("lowmemory");
            break;
        default:
            break;
    }

    return message;
}

love::Message * love::common::Event::ConvertJoystickEvent(const Hidrv::LOVE_Event & event) const
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
        case Hidrv::TYPE_GAMEPADUP:
        case Hidrv::TYPE_GAMEPADDOWN:
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

            message = new Message((event.type == Hidrv::TYPE_GAMEPADDOWN) ? "gamepadpressed" : "gamepadreleased", vargs);

            break;
        }
        case Hidrv::TYPE_GAMEPADAXIS:
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

love::Message * love::common::Event::ConvertWindowEvent(const Hidrv::LOVE_Event & event)
{
    Message * message = nullptr;

    std::vector<Variant> vargs;
    vargs.reserve(4);

    Window * windowModule = nullptr;

    if (event.type != Hidrv::TYPE_WINDOWEVENT)
        return nullptr;

    switch (event.subType)
    {
        case Hidrv::TYPE_FOCUS_LOST:
        case Hidrv::TYPE_FOCUS_GAINED:
        {
            vargs.emplace_back(event.type == Hidrv::TYPE_FOCUS_GAINED);

            message = new Message("focus", vargs);

            break;
        }
        case Hidrv::TYPE_RESIZE:
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

void love::common::Event::ExceptionIfInRenderPass(const char * name)
{} /* potentially useless */

void love::common::Event::InternalClear()
{
    Hidrv::LOVE_Event event;

    while (hidrv.Poll(&event))
    {
        /* do nothing */
    }

    this->Clear();
}

void love::common::Event::Clear()
{
    thread::Lock lock(this->mutex);

    while (!this->queue.empty())
    {
        this->queue.front()->Release();
        this->queue.pop();
    }
}

bool love::common::Event::Poll(Message *& message)
{
    thread::Lock lock(this->mutex);

    if (this->queue.empty())
        return false;

    message = this->queue.front();
    this->queue.pop();

    return true;
};

void love::common::Event::Push(Message * message)
{
    thread::Lock lock(this->mutex);

    message->Retain();
    this->queue.push(message);
}

love::Message * love::common::Event::Wait()
{
    Hidrv::LOVE_Event event;

    if (hidrv.Poll(&event) == false)
        return nullptr;

    return this->Convert(event);
}
