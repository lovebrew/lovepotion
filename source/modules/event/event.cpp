#include "common/runtime.h"
#include "common/backend/input.h"

#include "modules/event/event.h"

void love::Event::Clear()
{
    //Lock lock(this->mutex);

    while (!this->queue.empty())
    {
        this->queue.front()->Release();
        this->queue.pop();
    }
}

void love::Event::Pump()
{
    while (Input::PollEvent(&event))
    {
        Message * message = nullptr;

        Gamepad * gamepad = nullptr;
        love::Type * joystickType = &Gamepad::type;

        std::vector<Variant> vargs;
        vargs.reserve(4);

        auto joystickModule = Module::GetInstance<Joystick>(Module::M_JOYSTICK);

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
                vargs.emplace_back((float)gamepad->GetGamepadAxis(event.axis.axis));

                message = new Message("gamepadaxis", vargs);

                break;
            }
            case LOVE_TOUCHPRESS:
            case LOVE_TOUCHRELEASE:
            {
                vargs.emplace_back((void *)(intptr_t)event.touch.id);
                vargs.emplace_back((float)event.touch.x);
                vargs.emplace_back((float)event.touch.y);
                vargs.emplace_back((float)0.0f);
                vargs.emplace_back((float)0.0f);
                vargs.emplace_back((float)1.0f);

                message = new Message((event.type == LOVE_TOUCHPRESS) ?
                         "touchpressed" : "touchreleased", vargs);

                break;
            }
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

bool love::Event::Poll(Message *& message)
{
    //Lock lock(this->mutex);

    if (this->queue.empty())
        return false;

    message = this->queue.front();
    this->queue.pop();

    return true;
};

void love::Event::Push(Message * message)
{
    //Lock lock(this->mutex);

    message->Retain();
    this->queue.push(message);
}
