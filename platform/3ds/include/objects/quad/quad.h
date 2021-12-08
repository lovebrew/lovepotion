#pragma once

#include "objects/quad/quadc.h"
#include <c2d/base.h>

namespace love
{
    class Quad : public common::Quad
    {
      public:
        Quad(const Viewport& viewport, double sw, double sh);

        virtual ~Quad()
        {}

        void Refresh(const Viewport& viewport, double sw, double sh) override;

        const Tex3DS_SubTexture& CalculateTex3DSViewport(const Viewport& viewport,
                                                         C3D_Tex* texture);

      private:
        Tex3DS_SubTexture subTex;
    };
} // namespace love
