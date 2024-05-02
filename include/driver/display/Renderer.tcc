#pragma once

#include "common/Singleton.tcc"

#include "modules/graphics/Shader.tcc"
#include "modules/graphics/renderstate.hpp"
#include "modules/graphics/vertex.hpp"

#include "driver/graphics/DrawCommand.hpp"

#include <array>
#include <vector>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

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

        RendererBase()
        {}

        size_t getVertexCount() const
        {
            return this->renderCtx.vertexCount;
        }

        Vertex* getVertices()
        {
            return this->data;
        }

        virtual void prepareDraw() = 0;

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

            ShaderBase::StandardShader shader = ShaderBase::STANDARD_DEFAULT;
        };

        bool initialized = false;
        bool inFrame     = false;
    };
} // namespace love
