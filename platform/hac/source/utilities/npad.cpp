#include <utilities/npad.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/log/logfile.h>

using namespace love;

HidNpadStyleTag love::npad::GetStyleTag(PadState* state)
{
    uint32_t styleSet   = padGetStyleSet(state);
    uint32_t attributes = padGetAttributes(state);

    if (styleSet & HidNpadStyleTag_NpadFullKey)
        return HidNpadStyleTag_NpadFullKey;
    else if (styleSet & HidNpadStyleTag_NpadHandheld)
        return HidNpadStyleTag_NpadHandheld;

    /* this won't work with PadState split and join at the moment */
    if (styleSet & HidNpadStyleTag_NpadJoyLeft)
        return HidNpadStyleTag_NpadJoyLeft;

    if (styleSet & HidNpadStyleTag_NpadJoyDual)
    {
        if (!(attributes & HidNpadAttribute_IsLeftConnected))
            return HidNpadStyleTag_NpadJoyRight;
        else
            return HidNpadStyleTag_NpadJoyDual;
    }

    return INVALID_STYLE_TAG;
}

// clang-format off
constexpr BidirectionalMap styleTypes = {
    npad::INVALID_STYLE_TAG,      guid::GAMEPAD_TYPE_UNKNOWN,
    HidNpadStyleTag_NpadFullKey,  guid::GAMEPAD_TYPE_NINTENDO_SWITCH_PRO,
    HidNpadStyleTag_NpadHandheld, guid::GAMEPAD_TYPE_NINTENDO_SWITCH_HANDHELD,
    HidNpadStyleTag_NpadJoyLeft,  guid::GAMEPAD_TYPE_JOYCON_LEFT,
    HidNpadStyleTag_NpadJoyRight, guid::GAMEPAD_TYPE_JOYCON_RIGHT,
    HidNpadStyleTag_NpadJoyDual,  guid::GAMEPAD_TYPE_JOYCON_PAIR
};
// clang-format on

bool love::npad::GetConstant(HidNpadStyleTag in, guid::GamepadType& out)
{
    return styleTypes.Find(in, out);
}

bool love::npad::GetConstant(guid::GamepadType in, HidNpadStyleTag& out)
{
    return styleTypes.ReverseFind(in, out);
}
