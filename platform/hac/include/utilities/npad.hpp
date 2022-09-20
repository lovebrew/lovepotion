#pragma once

#include <switch.h>
#include <utilities/guid.hpp>

namespace love::npad
{
    static constexpr size_t MAX_JOYSTICKS              = 0x08;
    static constexpr HidNpadStyleTag INVALID_STYLE_TAG = (HidNpadStyleTag)-1;
    static constexpr HidNpadIdType INVALID_PLAYER_ID   = (HidNpadIdType)-1;

    HidNpadStyleTag GetStyleTag(PadState* state);

    bool GetConstant(guid::GamepadType in, HidNpadStyleTag& out);
    bool GetConstant(HidNpadStyleTag in, guid::GamepadType& out);
} // namespace love::npad
