#pragma once

#include <switch.h>
#include <utilities/guid.hpp>

namespace love::npad
{
    static constexpr size_t MAX_JOYSTICKS              = 0x08;
    static constexpr HidNpadStyleTag INVALID_STYLE_TAG = (HidNpadStyleTag)-1;
    static constexpr HidNpadIdType INVALID_PLAYER_ID   = (HidNpadIdType)-1;

    HidNpadStyleTag GetStyleTag(PadState* state);

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
} // namespace love::npad
