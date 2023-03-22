#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/font_ext.hpp>
#include <objects/rasterizer_ext.hpp>
#include <objects/shader_ext.hpp>

#include <utilities/driver/vertex_ext.hpp>

#include <memory>

namespace love
{
    template<>
    class Graphics<Console::CAFE> : public Graphics<Console::ALL>
    {
      public:
        static constexpr const char* DEFAULT_SCREEN = "tv";

        Graphics();

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth);

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void Present();

        bool SetMode(int x, int y, int width, int height);

        void SetColor(const Color& color);

        void SetFrontFaceWinding(vertex::Winding winding);

        void SetMeshCullMode(vertex::CullMode mode);

        void CheckSetDefaultFont();

        void RestoreStateChecked(const DisplayState& state);

        void SetBlendMode(RenderState::BlendMode mode, RenderState::BlendAlpha alphaMode);

        void SetBlendState(const RenderState::BlendState& state);

        void Pop();

        void Reset();

        void RestoreState(const DisplayState& state);

        Font<Console::CAFE>* NewFont(Rasterizer<Console::CAFE>* data) const;

        Font<Console::CAFE>* NewDefaultFont(int size,
                                            Rasterizer<Console::CAFE>::Hinting hinting) const;

        Texture<Console::CAFE>* NewTexture(const Texture<>::Settings& settings,
                                           const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::CAFE>* texture, Quad* quad,
                  const Matrix4<Console::CAFE>& matrix);

        void Draw(Drawable* drawable, const Matrix4<Console::CAFE>& matrix);

        void Print(const Font<>::ColoredStrings& strings, const Matrix4<Console::CAFE>& matrix);

        void Print(const Font<>::ColoredStrings& strings, Font<Console::CAFE>* font,
                   const Matrix4<Console::CAFE>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, float wrap, Font<>::AlignMode align,
                    const Matrix4<Console::CAFE>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, Font<Console::CAFE>* font, float wrap,
                    Font<>::AlignMode align, const Matrix4<Console::CAFE>& matrix);

        void SetScissor();

        void SetScissor(const Rect& scissor);

        void SetViewportSize(int width, int height);

        void SetShader()
        {
            Shader<Console::CAFE>::AttachDefault(Shader<>::STANDARD_DEFAULT);
            this->states.back().shader.Set(nullptr);
        }

        void SetShader(Shader<Console::CAFE>* shader)
        {
            if (shader == nullptr)
                return this->SetShader();

            shader->Attach();
            this->states.back().shader.Set(shader);
        }
    }; // namespace love
} // namespace love
