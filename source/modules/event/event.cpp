#include <modules/event/event.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <modules/joystickmodule_ext.hpp>
#include <modules/touch/touch.hpp>

#include <common/console.hpp>
#include <utilities/log/logfile.h>

#include <mutex>

using namespace love;

using HID            = love::HID<Console::Which>;
using JoystickModule = love::JoystickModule<Console::Which>;

Message* love::Event::Convert(const LOVE_Event& event)
{
    Message* message = nullptr;

    std::vector<Variant> args;
    args.reserve(4);

    switch (event.type)
    {
        case TYPE_GENERAL:
            message = this->ConvertGeneralEvent(event, args);
            break;
        case TYPE_GAMEPAD:
            message = this->ConvertJoystickEvent(event, args);
            break;
        case TYPE_TOUCH:
            message = this->ConvertTouchEvent(event, args);
            break;
        case TYPE_WINDOW:
            message = this->ConvertWindowEvent(event, args);
            break;
        default:
            break;
    }

    return message;
}

Message* love::Event::ConvertGeneralEvent(const LOVE_Event& event, std::vector<Variant>& args)
{
    Message* result = nullptr;

    switch (event.subType)
    {
        case SUBTYPE_QUIT:
            result = new Message("quit");
            break;
        case SUBTYPE_LOWMEMORY:
            result = new Message("lovememory");
            break;
        default:
            break;
    }

    return result;
}

Message* love::Event::ConvertWindowEvent(const LOVE_Event& event, std::vector<Variant>& args)
{
    // auto* windowModule = (::WindowModule*)Module::GetInstance<::WindowModule>(M_WINDOW);
    Message* result = nullptr;

    switch (event.subType)
    {
        case SUBTYPE_FOCUS_GAINED:
        case SUBTYPE_FOCUS_LOST:
        {
            args.emplace_back(event.subType == SUBTYPE_FOCUS_GAINED);

            result = new Message("focus", args);
            break;
        }
        default:
            break;
    }

    return result;
}

Message* love::Event::ConvertJoystickEvent(const LOVE_Event& event, std::vector<Variant>& args)
{
    auto* joystickModule = (::JoystickModule*)Module::GetInstance<::JoystickModule>(M_JOYSTICK);

    if (!joystickModule)
        return nullptr;

    auto* type           = &Joystick<>::type;
    ::Joystick* joystick = nullptr;
    const char* name     = nullptr;

    Message* result = nullptr;

    switch (event.subType)
    {
        case SUBTYPE_GAMEPADADDED:
        {
            joystick = joystickModule->AddJoystick(event.padStatus.id);

            if (!joystick)
                return result;

            args.emplace_back(type, joystick);

            result = new Message("joystickadded", args);
            break;
        }
        case SUBTYPE_GAMEPADREMOVED:
        {
            joystick = joystickModule->GetJoystickFromId(event.padStatus.id);

            if (!joystick)
                return result;

            joystickModule->RemoveJoystick(joystick);

            args.emplace_back(type, joystick);

            result = new Message("joystickremoved", args);
            break;
        }
        case SUBTYPE_GAMEPADDOWN:
        case SUBTYPE_GAMEPADUP:
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
            break;
        }
        case SUBTYPE_GAMEPADAXIS:
        {
            joystick = joystickModule->GetJoystickFromId(event.padButton.id);

            if (!joystick)
                return result;

            args.emplace_back(type, joystick);
            args.emplace_back(event.padAxis.name, strlen(event.padAxis.name));
            args.emplace_back(event.padAxis.value);

            result = new Message("gamepadaxis", args);
            break;
        }
        case SUBTYPE_GAMEPADUPDATED:
        {
            /* no Message, because why send that it was updated? */
            joystick = joystickModule->GetJoystickFromId(event.padButton.id);

            if (!joystick)
                return result;
            LOG("re-opening %zu", event.padStatus.id);
            joystick->Open(event.padButton.id);
            break;
        }
        default:
            break;
    }

    return result;
}

Message* love::Event::ConvertTouchEvent(const LOVE_Event& event, std::vector<Variant>& args)
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

void love::Event::InternalClear()
{
    LOVE_Event event;
    while (::HID::Instance().Poll(&event))
    {}

    this->Clear();
}

void love::Event::Push(Message* message)
{
    std::unique_lock lock(this->mutex);
    message->Retain();
    this->queue.push(message);
}

void love::Event::Pump()
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

bool love::Event::Poll(Message*& message)
{
    std::unique_lock lock(this->mutex);

    if (this->queue.empty())
        return false;

    message = this->queue.front();
    this->queue.pop();

    return true;
}

void love::Event::Clear()
{
    std::unique_lock lock(this->mutex);

    while (!this->queue.empty())
    {
        this->queue.front()->Release();
        this->queue.pop();
    }
}

Message* love::Event::Wait()
{
    // this->ExceptionIfInRenderPass("love.event.wait");

    LOVE_Event event;

    if (::HID::Instance().Poll(&event) == false)
        return nullptr;

    return this->Convert(event);
}
