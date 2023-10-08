#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/rasterizer/rasterizer.hpp>

namespace love
{
    template<>
    class Graphics<Console::CAFE> : public Graphics<Console::ALL>
    {
      public:
        static constexpr const char* DEFAULT_SCREEN = "tv";

        Graphics();

        bool SetMode(int x, int y, int width, int height);

        Texture<Console::CAFE>* NewTexture(const Texture<>::Settings& settings,
                                           const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::CAFE>* texture, Quad* quad,
                  const Matrix4<Console::CAFE>& matrix);

        void Draw(Drawable* drawable, const Matrix4<Console::CAFE>& matrix);

        void SetViewportSize(int width, int height);

        void SetShader();

        void SetShader(Shader<Console::CAFE>* shader);
    }; // namespace love
} // namespace love
