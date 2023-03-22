#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/font_ext.hpp>
#include <objects/rasterizer_ext.hpp>
#include <objects/shader_ext.hpp>

#include <utilities/driver/vertex_ext.hpp>

namespace love
{
    template<>
    class Graphics<Console::HAC> : public Graphics<Console::ALL>
    {
      public:
        Graphics();

        virtual ~Graphics();

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth);

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void Present();

        bool SetMode(int x, int y, int width, int height);

        void CheckSetDefaultFont();

        void RestoreStateChecked(const DisplayState& state);

        void Pop();

        void SetShader()
        {
            Shader<Console::HAC>::AttachDefault(Shader<>::STANDARD_DEFAULT);
            this->states.back().shader.Set(nullptr);
        }

        void SetShader(Shader<Console::HAC>* shader)
        {
            if (shader == nullptr)
                return this->SetShader();

            shader->Attach();
            this->states.back().shader.Set(shader);
        }

        void Reset();

        void RestoreState(const DisplayState& state);

        Font<Console::HAC>* NewFont(Rasterizer<Console::HAC>* data) const;

        Font<Console::HAC>* NewDefaultFont(int size,
                                           Rasterizer<Console::HAC>::Hinting hinting) const;

        Texture<Console::HAC>* NewTexture(const Texture<>::Settings& settings,
                                          const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::HAC>* texture, Quad* quad, const Matrix4<Console::HAC>& matrix);

        void Draw(Drawable* drawable, const Matrix4<Console::HAC>& matrix);

        void Print(const Font<>::ColoredStrings& strings, const Matrix4<Console::HAC>& matrix);

        void Print(const Font<>::ColoredStrings& strings, Font<Console::HAC>* font,
                   const Matrix4<Console::HAC>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, float wrap, Font<>::AlignMode align,
                    const Matrix4<Console::HAC>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, Font<Console::HAC>* font, float wrap,
                    Font<>::AlignMode align, const Matrix4<Console::HAC>& matrix);

        void SetScissor();

        void SetScissor(const Rect& scissor);

        void SetViewportSize(int width, int height);
    }; // namespace love
} // namespace love
