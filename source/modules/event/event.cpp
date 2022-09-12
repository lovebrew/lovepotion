#include <modules/event/event.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <modules/joystick_ext.hpp>
#include <modules/touch/touch.hpp>

#include <common/console.hpp>

#include <mutex>

using namespace love;

using HID            = love::HID<Console::Which>;
using JoystickModule = love::JoystickModule<Console::Which>;

Message* Event::Convert(const LOVE_Event& event)
{
    Message* message = nullptr;

    std::vector<Variant> args;
    args.reserve(4);

    switch (event.type)
    {
        case TYPE_GENERAL:
            break;
        case TYPE_GAMEPAD:
            message = this->ConvertJoystickEvent(event, args);
            break;
        case TYPE_TOUCH:
            message = this->ConvertTouchEvent(event, args);
            break;
        case TYPE_WINDOW:
            break;
    }

    return message;
}

Message* Event::ConvertJoystickEvent(const LOVE_Event& event, std::vector<Variant>& args)
{
    auto* joystickModule = (::JoystickModule*)Module::GetInstance<::JoystickModule>(M_JOYSTICK);

    if (!joystickModule)
        return nullptr;

    auto* type           = &Joystick<>::type;
    ::Joystick* joystick = nullptr;
    const char* name     = nullptr;

    Message* result = nullptr;

    if (event.subType == SUBTYPE_GAMEPADDOWN || event.subType == SUBTYPE_GAMEPADUP)
    {
        joystick = joystickModule->GetJoystickFromId(event.padButton.id);

        if (!joystick)
            return result;

        args.emplace_back(type, joystick);
        args.emplace_back(event.padButton.name, strlen(event.padButton.name));

        if (event.subType == SUBTYPE_GAMEPADDOWN)
            name = "gamepadpressed";
        else if (event.subType == SUBTYPE_GAMEPADUP)
            name = "gamepadreleased";

        result = new Message(name, args);
    }

    return result;
}

Message* Event::ConvertTouchEvent(const LOVE_Event& event, std::vector<Variant>& args)
{
    auto touch = (Touch*)Module::GetInstance<Touch>(M_TOUCH);
    Finger fingerInfo {};
    const char* name = nullptr;

    fingerInfo.id       = (int64_t)event.touchFinger.id;
    fingerInfo.x        = event.touchFinger.x;
    fingerInfo.y        = event.touchFinger.y;
    fingerInfo.dx       = event.touchFinger.dx;
    fingerInfo.dy       = event.touchFinger.dy;
    fingerInfo.pressure = event.touchFinger.pressure;

    if (touch)
        touch->OnEvent(event.subType, fingerInfo);

    args.emplace_back((void*)(intptr_t)fingerInfo.id);
    args.emplace_back((float)fingerInfo.x);
    args.emplace_back((float)fingerInfo.y);
    args.emplace_back((float)fingerInfo.dx);
    args.emplace_back((float)fingerInfo.dy);
    args.emplace_back((float)fingerInfo.pressure);

    if (event.subType == SUBTYPE_TOUCHPRESS)
        name = "touchpressed";
    else if (event.subType == SUBTYPE_TOUCHRELEASE)
        name = "touchreleased";
    else
        name = "touchmoved";

    return new Message(name, args);
}

void Event::InternalClear()
{
    LOVE_Event event;
    while (::HID::Instance().Poll(&event))
    {}

    this->Clear();
}

void Event::Push(Message* message)
{
    std::unique_lock lock(this->mutex);
    message->Retain();
    this->queue.push(message);
}

void Event::Pump()
{
    while (::HID::Instance().Poll(&this->event))
    {
        Message* message = this->Convert(this->event);

        if (message)
        {
            this->Push(message);
            message->Release();
        }
    }
}

bool Event::Poll(Message*& message)
{
    std::unique_lock lock(this->mutex);

    if (this->queue.empty())
        return false;

    message = this->queue.front();
    this->queue.pop();

    return true;
}

void Event::Clear()
{
    std::unique_lock lock(this->mutex);

    while (!this->queue.empty())
    {
        this->queue.front()->Release();
        this->queue.pop();
    }
}

Message* Event::Wait()
{
    // this->ExceptionIfInRenderPass("love.event.wait");

    LOVE_Event event;

    if (::HID::Instance().Poll(&event) == false)
        return nullptr;

    return this->Convert(event);
}
