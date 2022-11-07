#pragma once

#include <common/console.hpp>
#include <common/math.hpp>

namespace love
{
    class Canvas;
    class Texture;

    template<Console::Platform T = Console::ALL>
    class Renderer
    {
      public:
        static int shaderSwitches;
        static int drawCalls;

        struct Info
        {
            std::string_view name;
            std::string_view version;
            std::string_view vendor;
            std::string_view device;
        };

        Renderer() : info {}, inFrame(false), viewport {}
        {}

      protected:
        Info info;
        bool inFrame;
        Rect viewport;
    };
} // namespace love
