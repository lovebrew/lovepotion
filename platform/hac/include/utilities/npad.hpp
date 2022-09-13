#pragma once

#include <switch.h>
#include <utilities/guid.hpp>

namespace love
{
    HidNpadStyleTag GetStyleTag(PadState* state);

    bool GetConstant(guid::GamepadType in, HidNpadStyleTag& out);
    bool GetConstant(HidNpadStyleTag in, guid::GamepadType& out);
} // namespace love
