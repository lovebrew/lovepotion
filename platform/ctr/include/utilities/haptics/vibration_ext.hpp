#pragma once

#include <utilities/haptics/vibration.tcc>

namespace love
{
    template<>
    class Vibration<Console::CTR> : public Vibration<Console::ALL>
    {
    };
} // namespace love
