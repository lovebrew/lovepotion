#pragma once

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/Volatile.hpp"

#include <citro3d.h>

namespace love
{
    class Texture final : public TextureBase, public Volatile
    {
      public:
        Texture(Graphics& graphics, const Settings& settings, const Slices* data);

        virtual ~Texture();

        bool loadVolatile() override;

        void unloadVolatile() override;

        ptrdiff_t getHandle() const override;

        void draw(Graphics& graphics, const Matrix4& matrix);

        void draw(Graphics& graphics, Quad* quad, const Matrix4& matrix);

        void setSamplerState(const SamplerState& state);

        SamplerState validateSamplerState(SamplerState state) const;

        bool validateDimensions(bool throwException) const;

        void validatePixelFormat(Graphics& graphics) const;

      private:
        void createTexture();

        Slices slices;

        C3D_Tex* texture         = nullptr;
        C3D_RenderTarget* target = nullptr;
    };
} // namespace love
