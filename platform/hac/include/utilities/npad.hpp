#pragma once

#include <switch.h>
#include <utilities/guid.hpp>

namespace love::npad
{
    static constexpr size_t MAX_JOYSTICKS = 0x08;

    HidNpadStyleTag GetStyleTag(PadState* state);

    bool GetConstant(guid::GamepadType in, HidNpadStyleTag& out);
    bool GetConstant(HidNpadStyleTag in, guid::GamepadType& out);
} // namespace love::npad
