#pragma once

#include "objects/canvas/canvasc.h"

namespace love
{
    class Canvas : public common::Canvas
    {
        public:
            Canvas(const Settings & settings);
            ~Canvas();

            void Draw(Graphics * gfx, Quad * quad, const Matrix4 & localTransform) override;

            void Clear(const Colorf & color);

        private:
            C3D_Tex citroTex;
    };
}