#pragma once

#include <objects/source/source.tcc>

namespace love
{
    template<>
    class Source<Console::HAC> : public Source<Console::ALL>
    {
    };
} // namespace love
