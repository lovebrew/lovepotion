#pragma once

#include <common/console.hpp>
#include <common/math.hpp>

namespace love
{
    class Canvas;
    class Texture;

    enum class Screen : uint8_t;

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

            bool filled = false;
        };

        Renderer() : info {}, inFrame(false), viewport {}
        {}

        Rect GetViewport() const
        {
            return this->viewport;
        }

      protected:
        Info info;

        bool inFrame;
        Rect viewport;
    };
} // namespace love
