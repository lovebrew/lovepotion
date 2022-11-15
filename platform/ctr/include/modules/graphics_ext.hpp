#pragma once

#include <modules/graphics/graphics.tcc>

namespace love
{
    template<>
    class Graphics<Console::CTR> : public Graphics<Console::ALL>
    {
      public:
        Graphics();

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth);

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void Present();

        void SetMode(int x, int y, int width, int height);

        float GetCurrentDepth() const
        {
            return CURRENT_DEPTH;
        }

        void PushCurrentDepth()
        {
            CURRENT_DEPTH += MIN_DEPTH;
        }

      private:
        static inline float CURRENT_DEPTH = 0.0f;
        static constexpr float MIN_DEPTH  = 1.0f / 16384.0f;
    };
} // namespace love
