#pragma once

#include "common/math.hpp"

#include <cstdint>

namespace love
{
    class FramebufferBase
    {
      public:
        static constexpr float Z_NEAR = -10.0f;
        static constexpr float Z_FAR  = 10.0f;

        int getWidth() const
        {
            return this->width;
        }

        int getHeight() const
        {
            return this->height;
        }

        Rect getScissor()
        {
            return this->scissor;
        }

        Rect getViewport()
        {
            return this->viewport;
        }

      protected:
        int width;
        int height;

        Rect scissor;
        Rect viewport;
    };
} // namespace love
