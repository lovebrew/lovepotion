#pragma once

#include "common/colors.h"
#include "objects/canvas/canvasc.h"
#include <citro2d.h>

namespace love
{
    class Canvas : public common::Canvas
    {
      public:
        Canvas(const Settings& settings);

        virtual ~Canvas();

        void Draw(Graphics* gfx, Quad* quad, const Matrix4& localTransform) override;

        void Clear(const Colorf& color);

        C3D_RenderTarget* GetRenderer();

      private:
        C3D_Tex citroTex;
        Tex3DS_SubTexture subtex;
        C3D_RenderTarget* renderer;
    };
} // namespace love
