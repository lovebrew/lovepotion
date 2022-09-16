#pragma once

#include <padscore/wpad.h>
#include <utilities/guid.hpp>

namespace love::wpad
{
    guid::GamepadType GetWPADType(WPADExtensionType extension);
} // namespace love::wpad
