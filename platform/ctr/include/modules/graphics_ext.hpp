#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/rasterizer_ext.hpp>

namespace love
{
    template<>
    class Graphics<Console::CTR> : public Graphics<Console::ALL>
    {
      public:
        static constexpr const char* DEFAULT_SCREEN = "top";

        Graphics();

        void SetMode(int x, int y, int width, int height);

        void CheckSetDefaultFont();

        Font<Console::CTR>* NewDefaultFont(int size) const;

        void SetScissor();

        void SetScissor(const Rect& rectangle);

        void Set3D(bool enabled);

        bool Get3D();

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
