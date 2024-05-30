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

        ptrdiff_t getRenderTargetHandle() const override;

        void updateQuad(Quad* quad);

        void setSamplerState(const SamplerState& state) override;

        void uploadByteData(const void* data, size_t size, int slice, int mipmap, const Rect& rect) override;

        void generateMipmapsInternal() override;

        void setHandleData(void* data) override
        {
            this->texture->data = data;
        }

        // bool validateDimensions(bool throwException) const;

        // void validatePixelFormat(Graphics& graphics) const;

      private:
        void createTexture();

        Slices slices;

        C3D_Tex* texture         = nullptr;
        C3D_RenderTarget* target = nullptr;
    };
} // namespace love
