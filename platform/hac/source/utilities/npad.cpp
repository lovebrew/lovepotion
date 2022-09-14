#include <utilities/npad.hpp>

#include <utilities/bidirectionalmap.hpp>

static constexpr auto INVALID_STYLE_TAG = (HidNpadStyleTag)-1;

using namespace love;

HidNpadStyleTag love::npad::GetStyleTag(PadState* state)
{
    uint32_t styleSet   = padGetStyleSet(state);
    uint32_t attributes = padGetAttributes(state);

    if (styleSet & HidNpadStyleTag_NpadFullKey)
        return HidNpadStyleTag_NpadFullKey;
    else if (styleSet & HidNpadStyleTag_NpadHandheld)
        return HidNpadStyleTag_NpadHandheld;
    else if (styleSet & HidNpadStyleTag_NpadJoyLeft)
        return HidNpadStyleTag_NpadJoyLeft;
    else if (styleSet & HidNpadStyleTag_NpadJoyRight)
        return HidNpadStyleTag_NpadJoyRight;
    else if (styleSet & HidNpadStyleTag_NpadJoyDual)
        return HidNpadStyleTag_NpadJoyDual;

    return INVALID_STYLE_TAG;
}

// clang-format off
constexpr auto styleTypes = BidirectionalMap<>::Create(
    HidNpadStyleTag_NpadFullKey,  guid::GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
    HidNpadStyleTag_NpadHandheld, guid::GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD,
    HidNpadStyleTag_NpadJoyDual,  guid::GAMEPAD_TYPE_JOYCON_PAIR,
    HidNpadStyleTag_NpadJoyLeft,  guid::GAMEPAD_TYPE_JOYCON_LEFT,
    HidNpadStyleTag_NpadJoyRight, guid::GAMEPAD_TYPE_JOYCON_RIGHT
);
// clang-format on

bool love::npad::GetConstant(HidNpadStyleTag in, guid::GamepadType& out)
{
    return styleTypes.Find(in, out);
}

bool love::npad::GetConstant(guid::GamepadType in, HidNpadStyleTag& out)
{
    return styleTypes.ReverseFind(in, out);
}
