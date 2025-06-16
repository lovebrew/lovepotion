#pragma once

#include "modules/graphics/Graphics.tcc"

namespace love
{
    class Graphics final : public GraphicsBase
    {
      public:
        Graphics();

        virtual ~Graphics();

        void initCapabilities() override;

        void clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth) override;

        void clear(const std::vector<OptionalColor>& colors, OptionalInt stencil,
                   OptionalDouble depth) override;

        void present(void* screenshotCallbackData) override;

        void setScissor(const Rect& scissor) override;

        void setScissor() override;

        void setFrontFaceWinding(Winding winding) override;

        void setColorMask(ColorChannelMask mask) override;

        void setBlendState(const BlendState& state) override;

        void setPointSize(float size) override;

        FontBase* newFont(Rasterizer* data) override;

        FontBase* newDefaultFont(int size, const Rasterizer::Settings& settings) override;

        TextureBase* newTexture(const TextureBase::Settings& settings,
                                const TextureBase::Slices* data = nullptr) override;

        ShaderStageBase* newShaderStageInternal(ShaderStageType stage, const std::string& filepath) override;

        ShaderBase* newShaderInternal(StrongRef<ShaderStageBase> stages[SHADERSTAGE_MAX_ENUM],
                                      const ShaderBase::CompileOptions& options) override;

        bool setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                     bool backBufferDepth, int msaa) override;

        void setRenderTargetsInternal(const RenderTargets& targets, int pixelWidth, int pixelHeight,
                                      bool hasSRGBTexture) override;

        bool isPixelFormatSupported(PixelFormat format, uint32_t usage) override;

        void draw(const DrawIndexedCommand& command) override;

        void draw(const DrawCommand& command) override;

        using GraphicsBase::draw;

        void unsetMode() override;

        void setActiveScreen() override;

        void setViewport(int x, int y, int width, int height);

      private:
        bool backBufferHasStencil;
        bool backBufferHasDepth;
        int requestedBackbufferMSAA;
    };
} // namespace love
