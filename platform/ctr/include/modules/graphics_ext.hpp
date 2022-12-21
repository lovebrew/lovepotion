#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/rasterizer_ext.hpp>

namespace love
{
    template<>
    class Graphics<Console::CTR> : public Graphics<Console::ALL>
    {
      public:
        Graphics();

        void Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth);

        void Clear(std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void Present();

        void Reset();

        void RestoreState(const DisplayState& state);

        void SetMode(int x, int y, int width, int height);

        void CheckSetDefaultFont();

        Font<Console::CTR>* NewFont(Rasterizer<Console::CTR>* data) const;

        Font<Console::CTR>* NewDefaultFont(int size) const;

        Font<Console::CTR>* GetFont();

        Texture<Console::CTR>* NewTexture(const Texture<>::Settings& settings,
                                          const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::CTR>* texture, Quad* quad, const Matrix4<Console::CTR>& matrix);

        void Draw(Drawable* drawable, const Matrix4<Console::CTR>& matrix);

        void Print(const Font<>::ColoredStrings& strings, const Matrix4<Console::CTR>& matrix);

        void Print(const Font<>::ColoredStrings& strings, Font<Console::CTR>* font,
                   const Matrix4<Console::CTR>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, float wrap, Font<>::AlignMode align,
                    const Matrix4<Console::CTR>& matrix);

        void Printf(const Font<>::ColoredStrings& strings, Font<Console::CTR>* font, float wrap,
                    Font<>::AlignMode align, const Matrix4<Console::CTR>& matrix);

        void SetScissor(const Rect& scissor);

        void RestoreState(const DisplayState& state)
        {
            this->SetFont(state.font.Get());
            Graphics<>::RestoreState(state);
        }

        int GetWidth(Screen screen) const;

        int GetHeight() const;

        float GetCurrentDepth() const
        {
            return CURRENT_DEPTH;
        }

        void PushCurrentDepth()
        {
            CURRENT_DEPTH += MIN_DEPTH;
        }

      private:
        static inline float CURRENT_DEPTH = 0.0f;
        static constexpr float MIN_DEPTH  = 1.0f / 16384.0f;
    };
} // namespace love
