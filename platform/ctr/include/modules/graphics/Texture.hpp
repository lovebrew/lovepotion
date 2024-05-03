#pragma once

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/Volatile.hpp"

#include <citro3d.h>

namespace love
{
    class Texture final : public TextureBase, public Volatile
    {
      public:
        Texture(GraphicsBase* graphics, const Settings& settings, const Slices* data);

        virtual ~Texture();

        bool loadVolatile() override;

        void unloadVolatile() override;

        ptrdiff_t getHandle() const override;

        void updateQuad(Quad* quad);

        void setSamplerState(const SamplerState& state) override;

        void replacePixels(ImageDataBase* data, int slice, int mipmap, int x, int y,
                           bool reloadMipmaps) override;

        void replacePixels(const void* data, size_t size, int slice, int mipmap, const Rect& rect,
                           bool reloadMipmaps) override;

        SamplerState validateSamplerState(SamplerState state) const;

        // bool validateDimensions(bool throwException) const;

        // void validatePixelFormat(Graphics& graphics) const;

      private:
        void createTexture();

        Slices slices;

        C3D_Tex* texture         = nullptr;
        C3D_RenderTarget* target = nullptr;
    };
} // namespace love
