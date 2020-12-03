#pragma once

#include "objects/canvas/canvasc.h"

namespace love
{
    class Canvas : public common::Canvas
    {
        public:
            Canvas(const Settings & settings);

            void SetAsTarget() override;

            void Clear(const Colorf & color) override;

        private:
            C3D_Tex citroTex;
    };
}