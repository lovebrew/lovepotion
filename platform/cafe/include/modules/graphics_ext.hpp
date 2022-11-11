#pragma once

#include <modules/graphics/graphics.tcc>

namespace love
{
    template<>
    class Graphics<Console::CAFE> : public Graphics<Console::ALL>
    {
      public:
        Graphics();

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth);

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void Present();

        void SetMode(int x, int y, int width, int height);
    };
} // namespace love
