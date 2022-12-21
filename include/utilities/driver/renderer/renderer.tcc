#pragma once

#include <common/console.hpp>
#include <common/math.hpp>

#include <utilities/bidirectionalmap/smallvector.hpp>

#define DK_HPP_SUPPORT_VECTOR

namespace love
{
    class Canvas;

    template<Console::Platform T>
    class Texture;

    enum class Screen : uint8_t;

    template<Console::Platform T = Console::ALL>
    class Renderer
    {
      public:
        static inline int shaderSwitches = 0;
        static inline int drawCalls      = 0;

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
