#pragma once

#include <common/console.hpp>
#include <common/math.hpp>

#include <common/screen.hpp>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Framebuffer
    {
      public:
        Framebuffer() : width(0), height(0), scissor {}, viewport {}
        {}

        int GetWidth() const
        {
            return this->width;
        }

        int GetHeight() const
        {
            return this->height;
        }

        Rect GetViewport()
        {
            return this->viewport;
        }

        Rect GetScissor()
        {
            return this->scissor;
        }

      protected:
        static constexpr float Z_NEAR = -10.0f;
        static constexpr float Z_FAR  = 10.0f;

        Screen id;

        int width;
        int height;

        Rect scissor;
        Rect viewport;
    };
} // namespace love