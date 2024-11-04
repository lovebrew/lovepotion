#pragma once

#include "common/Singleton.tcc"

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Shader.tcc"
#include "modules/graphics/renderstate.hpp"

#include "modules/graphics/vertex.hpp"

#include <array>
#include <vector>

namespace love
{
    class RendererBase
    {
      public:
        RendererBase()
        {}

        virtual void prepareDraw(GraphicsBase* graphics) = 0;

        bool isInFrame() const
        {
            return this->inFrame;
        }

      protected:
        struct ContextBase
        {
            bool dirtyProjection;

            CullMode cullMode;
            ColorChannelMask colorMask;
            BlendState blendState;
            StencilState stencilState;

            Rect scissor;
            Rect viewport;
        };

        bool initialized = false;
        bool inFrame     = false;
    };
} // namespace love
