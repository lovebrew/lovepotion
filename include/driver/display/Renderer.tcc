#pragma once

#include "common/Singleton.tcc"

#include "modules/graphics/renderstate.hpp"
#include "modules/graphics/vertex.hpp"

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

        size_t getVertexCount() const
        {
            return this->renderCtx.vertexCount;
        }

        Vertex* getVertices()
        {
            return this->data;
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

        Vertex* data;

        struct RenderContext
        {
            size_t vertexCount;
            size_t indexCount;
        } renderCtx;
    };
} // namespace love
