#pragma once

#include <common/console.hpp>
#include <common/math.hpp>

#include <utilities/bidirectionalmap/smallvector.hpp>
#include <utilities/driver/renderer/renderstate.hpp>

#define DK_HPP_SUPPORT_VECTOR

namespace love
{
    class Canvas;

    template<Console::Platform T>
    class Texture;

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

        static constexpr float Z_NEAR = -10.0f;
        static constexpr float Z_FAR  = 10.0f;

        Renderer() : info {}, inFrame(false), viewport {}
        {}

        Rect GetViewport() const
        {
            return this->viewport;
        }

        void SetLineWidth(float width)
        {}

        void SetLineStyle(RenderState::LineStyle style)
        {}

        void SetPointSize(float size)
        {}

      protected:
        Info info;

        bool inFrame;
        Rect viewport;
    };
} // namespace love
