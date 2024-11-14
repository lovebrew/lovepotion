#pragma once

#include "modules/graphics/Texture.tcc"
#include "modules/graphics/Volatile.hpp"

#include "driver/display/deko3d/CMemPool.h"
#include <deko3d.hpp>

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

        dk::ImageDescriptor& getDescriptorHandle()
        {
            return this->descriptor;
        }

        dk::SamplerDescriptor& getSamplerDescriptor()
        {
            return this->samplerDescriptor;
        }

        void setSamplerState(const SamplerState& state) override;

        void uploadByteData(const void* data, size_t size, int slice, int mipmap, const Rect& rect) override;

        void generateMipmapsInternal() override;

        void setHandleData(ptrdiff_t data) override
        {
            this->handle = (DkResHandle)data;
        }

      private:
        void createTexture();

        Slices slices;

        dk::Image image;
        dk::ImageDescriptor descriptor;
        dk::Sampler sampler;
        dk::SamplerDescriptor samplerDescriptor;

        DkResHandle handle;
        CMemPool::Handle memory;
    };
} // namespace love
