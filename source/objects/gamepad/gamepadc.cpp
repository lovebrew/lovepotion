#include "objects/gamepad/gamepadc.h"
#include "common/bidirectionalmap.h"

using namespace love::common;

love::Type Gamepad::type("Joystick", &Object::type);

Gamepad::Gamepad(size_t id) : id(id), instanceID(-1), vibration()
{}

float Gamepad::ClampValue(float x)
{
    if (std::abs(x) < 0.01)
        return 0.0f;

    if (x < -0.99f)
        return -1.0f;
    if (x > 0.99f)
        return 1.0f;

    return x;
}

std::string Gamepad::GetGUID() const
{
    return this->guid;
}

size_t Gamepad::GetInstanceID() const
{
    return this->instanceID;
}

size_t Gamepad::GetID() const
{
    return this->id;
}

bool Gamepad::IsGamepad() const
{
    return true;
}

// clang-format off
constexpr auto inputTypes = BidirectionalMap<>::Create(
    "axis",   Gamepad::INPUT_TYPE_AXIS,
    "button", Gamepad::INPUT_TYPE_BUTTON
);
// clang-format on

bool Gamepad::GetConstant(const char* in, Gamepad::InputType& out)
{
    return inputTypes.Find(in, out);
}

bool Gamepad::GetConstant(Gamepad::InputType in, const char*& out)
{
    return inputTypes.ReverseFind(in, out);
}
