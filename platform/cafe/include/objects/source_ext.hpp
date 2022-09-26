#pragma once

#include <objects/source/source.tcc>

namespace love
{
    template<>
    class Source<Console::CAFE> : public Source<Console::ALL>
    {
    };
} // namespace love
