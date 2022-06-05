#pragma once

#include "deko3d/CImage.h"
#include "objects/texture/texturec.h"

namespace love
{
    class Texture : public common::Texture
    {
      public:
        Texture(TextureType type);

        virtual ~Texture();

        void SetHandle(DkResHandle handle);

        DkResHandle GetHandle();

        static constexpr int TEXTURE_QUAD_POINT_COUNT = 4;

        void Draw(Graphics* gfx, const Matrix4& localTransform) override;

        void Draw(Graphics* gfx, love::Quad* quad, const Matrix4& localTransform) override;

        virtual void SetSamplerState(const SamplerState& state) override;

      protected:
        DkResHandle handle;
        CImage texture;
    };
} // namespace love
