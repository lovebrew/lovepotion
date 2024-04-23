#pragma once

#include "common/Singleton.tcc"

#include "modules/graphics/renderstate.hpp"

#include <array>

namespace love
{
    template<class T>
    class RendererBase : public Singleton<T>
    {
      public:
        enum RENDERER_INFO
        {
            RENDERER_INFO_NAME,
            RENDERER_INFO_VERSION,
            RENDERER_INFO_VENDOR,
            RENDERER_INFO_DEVICE
        };

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
