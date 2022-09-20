#include <objects/joystick/wrap_joystick.hpp>
#include <utilities/npad.hpp>

#include <utilities/log/logfile.h>

using namespace love;

#define instance() (Module::GetInstance<JoystickModule<Console::HAC>>(Module::M_JOYSTICK))

static HidNpadIdType luaL_checkIdType(lua_State* L, int index)
{
    int playerId = luaL_checknumber(L, index);

    if (playerId < 0 || playerId >= npad::MAX_JOYSTICKS)
        luaL_error(L, "invalid player id %d", playerId);

    return (HidNpadIdType)playerId;
}

int Wrap_Joystick::Split(lua_State* L)
{
    auto* self = Wrap_Joystick::CheckJoystick(L, 1);

    HidNpadIdType id = (HidNpadIdType)self->GetPlayerIndex();

    HidNpadIdType left  = luaL_checkIdType(L, 2);
    HidNpadIdType right = luaL_checkIdType(L, 3);

    bool setOutput = true;

    /* set the left */
    auto device = HidNpadJoyDeviceType_Left;
    auto result = hidSetNpadJoyAssignmentModeSingleWithDestination(id, device, &setOutput, &left);

    if (R_FAILED(result))
        return 0;

    /* set the right */
    device = HidNpadJoyDeviceType_Right;
    hidSetNpadJoyAssignmentModeSingleWithDestination(right, device, &setOutput, &right);

    return 0;
}

int Wrap_Joystick::Join(lua_State* L)
{
    auto* self        = Wrap_Joystick::CheckJoystick(L, 1);
    ::Joystick* other = nullptr;

    if (luax::IsType(L, 2, ::Joystick::type))
        other = Wrap_Joystick::CheckJoystick(L, 2);
    else if (lua_isnumber(L, 2))
        other = instance()->GetJoystickFromId(luaL_checknumber(L, 2) - 1);
    else
        luax::TypeError(L, 2, "Joystick or number");

    HidNpadIdType first = (HidNpadIdType)self->GetPlayerIndex();
    HidNpadIdType second;

    if (other != nullptr)
        second = (HidNpadIdType)other->GetPlayerIndex();
    else
        return luaL_error(L, "Joystick does not exist.");

    /* mark left to be merged */
    auto result = hidSetNpadJoyAssignmentModeDual(first);

    if (R_FAILED(result))
        return 0;

    /* mark right to merge */
    result = hidSetNpadJoyAssignmentModeDual(second);

    if (R_FAILED(result))
        return 0;

    /* merge both of the controllers now -- they become the id of "first" */
    result = hidMergeSingleJoyAsDualJoy(first, second);

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "split", Wrap_Joystick::Split },
    { "join",  Wrap_Joystick::Join  }
};
// clang-format on

std::span<const luaL_Reg> Wrap_Joystick::extension(functions);
