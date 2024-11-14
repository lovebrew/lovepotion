#pragma once

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/Volatile.hpp"

#include <gx2/sampler.h>
#include <gx2/texture.h>

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

        ptrdiff_t getSamplerHandle() const override;

        void setSamplerState(const SamplerState& state) override;

        void uploadByteData(const void* data, size_t size, int slice, int mipmap, const Rect& rect) override;

        void generateMipmapsInternal() override;

        void setHandleData(ptrdiff_t data) override
        {}

      private:
        void createTexture();

        Slices slices;

        GX2Texture* texture    = nullptr;
        GX2ColorBuffer* target = nullptr;
        GX2Sampler sampler;
    };
} // namespace love
