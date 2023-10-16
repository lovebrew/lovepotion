#pragma once

#include <padscore/kpad.h>
#include <utilities/guid.hpp>

namespace love::wpad
{
    static constexpr size_t MAX_JOYSTICKS = 4;

    guid::GamepadType GetWPADType(KPADExtensionType extension);
} // namespace love::wpad
