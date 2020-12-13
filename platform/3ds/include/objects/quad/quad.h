#pragma once

#include "objects/quad/quadc.h"

namespace love
{
    class Quad : public common::Quad
    {
        public:
            Quad(const Viewport & viewport, double sw, double sh);

            void Refresh(const Viewport & viewport, double sw, double sh) override;

            const Tex3DS_SubTexture & GetTex3DSViewport() const;

        private:
            Tex3DS_SubTexture subTex;
    };
}