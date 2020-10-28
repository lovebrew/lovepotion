#pragma once

#include "modules/graphics/graphics.h"
#include "deko3d/deko.h"

namespace love::deko3d
{
    class Graphics : public love::Graphics
    {
        public:
            Graphics();

            void Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth) override;

            void Present() override;

            void SetScissor(const Rect & scissor) override;

            void SetScissor() override;

            void Rectangle(const std::string & mode, float x, float y, float width, float height, float rx, float ry) override;

            void SetBlendMode(BlendMode mode, BlendAlpha alpha) override;

            void SetColorMask(ColorMask mask) override;

            // Internal?
            Shader * NewShader(void * vertex, size_t vertex_sz, void * pixel, size_t pixel_sz);

            ShaderStage * NewShaderStage(ShaderStage::StageType stage, void * data, size_t size);
    };
}