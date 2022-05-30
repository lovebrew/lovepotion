#pragma once

#include "objects/texture/texturec.h"
#include <citro2d.h>

namespace love
{
    class Texture : public common::Texture
    {
      public:
        Texture(TextureType type);

        virtual ~Texture()
        {}

        const C2D_Image& GetHandle();

        void Draw(Graphics* gfx, const Matrix4& localTransform) override;

        void Draw(Graphics* gfx, love::Quad* quad, const Matrix4& localTransform) override;

        void SetSamplerState(const SamplerState& state);

      protected:
        C2D_Image texture;
        C2D_SpriteSheet sheet;
    };
} // namespace love
