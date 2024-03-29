#pragma once

#include <modules/graphics/graphics.tcc>

namespace love
{
    template<>
    class Graphics<Console::CTR> : public Graphics<Console::ALL>
    {
      public:
        static inline uint32_t TRANSPARENCY         = Color(Color::CTR_TRANSPARENCY).rgba();
        static constexpr const char* DEFAULT_SCREEN = "top";

        Graphics();

        void SetMode(int x, int y, int width, int height);

        /* objects */

        Texture<Console::CTR>* NewTexture(const Texture<>::Settings& settings,
                                          const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::CTR>* texture, Quad* quad, const Matrix4& matrix);

        void Draw(Drawable* drawable, const Matrix4& matrix);

        void SetShader();

        void SetShader(Shader<Console::CTR>* shader);

        /* specific stuff */

        void Set3D(bool enabled);

        bool Get3D();

        static void ResetDepth()
        {
            CURRENT_DEPTH = 0.0f;
        }

        float GetCurrentDepth() const
        {
            return CURRENT_DEPTH;
        }

        float PushCurrentDepth(float mul = 1.0f)
        {
            return CURRENT_DEPTH + MIN_DEPTH * mul;
        }

      private:
        static inline float CURRENT_DEPTH = 0.0f;
        static constexpr float MIN_DEPTH  = 1.0f / 16384.0f;
    };
} // namespace love
