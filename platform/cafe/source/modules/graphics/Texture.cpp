#include "driver/display/GX2.hpp"

#include "modules/graphics/Texture.hpp"

#include <gx2/utils.h>

#include <malloc.h>

namespace love
{
    static void createTextureObject(GX2Texture*& texture, PixelFormat format, int width, int height)
    {
        texture = new GX2Texture();

        if (!texture)
            throw love::Exception("Failed to create GX2Texture.");

        std::memset(texture, 0, sizeof(GX2Texture));

        texture->surface.use    = GX2_SURFACE_USE_TEXTURE;
        texture->surface.dim    = GX2_SURFACE_DIM_TEXTURE_2D;
        texture->surface.width  = width;
        texture->surface.height = height;

        texture->surface.depth     = 1;
        texture->surface.mipLevels = 1;

        GX2SurfaceFormat gpuFormat;
        if (!GX2::getConstant(format, gpuFormat))
            throw love::Exception("Invalid pixel format {:s}.", love::getConstant(format));

        texture->surface.format   = gpuFormat;
        texture->surface.aa       = GX2_AA_MODE1X;
        texture->surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
        texture->viewFirstMip     = 0;
        texture->viewNumMips      = 1;
        texture->viewFirstSlice   = 0;
        texture->viewNumSlices    = 1;
        texture->compMap          = GX2_COMP_MAP(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A);

        GX2CalcSurfaceSizeAndAlignment(&texture->surface);
        GX2InitTextureRegs(texture);

        texture->surface.image = memalign(texture->surface.alignment, texture->surface.imageSize);

        if (!texture->surface.image)
            throw love::Exception("Failed to allocate texture memory.");

        std::memset(texture->surface.image, 0, texture->surface.imageSize);
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture->surface.image, texture->surface.imageSize);
    }

    Texture::Texture(GraphicsBase* graphics, const Settings& settings, const Slices* data) :
        TextureBase(graphics, settings, data),
        slices(settings.type),
        sampler(nullptr)
    {
        if (data != nullptr)
            slices = *data;

        if (!this->loadVolatile())
            throw love::Exception("Failed to create texture.");

        slices.clear();
    }

    Texture::~Texture()
    {
        this->unloadVolatile();
    }

    bool Texture::loadVolatile()
    {
        if (this->texture != nullptr || this->target != nullptr)
            return true;

        if (this->parentView.texture != this)
        {
            Texture* baseTexture = (Texture*)this->parentView.texture;
            baseTexture->loadVolatile();
        }

        if (this->isReadable())
            this->createTexture();

        int64_t memorySize = 0;

        for (int mip = 0; mip < this->getMipmapCount(); mip++)
        {
            int width  = this->getPixelWidth(mip);
            int height = this->getPixelHeight(mip);

            const auto faces = (this->textureType == TEXTURE_CUBE) ? 6 : 1;
            int slices       = this->getDepth(mip) * this->layers * faces;

            memorySize += getPixelFormatSliceSize(this->format, width, height) * slices;
        }

        this->setGraphicsMemorySize(memorySize);

        return true;
    }

    void Texture::unloadVolatile()
    {
        if (this->texture != nullptr)
            delete this->texture;

        if (this->target != nullptr)
            delete this->texture;

        this->setGraphicsMemorySize(0);
    }

    void Texture::createTexture()
    {
        if (!this->isRenderTarget())
        {
            try
            {
                createTextureObject(this->texture, this->format, this->pixelWidth, this->pixelHeight);
            }
            catch (love::Exception&)
            {
                throw;
            }

            int mipCount   = this->getMipmapCount();
            int sliceCount = 1;

            if (this->textureType == TEXTURE_VOLUME)
                sliceCount = this->getDepth();
            else if (this->textureType == TEXTURE_2D_ARRAY)
                sliceCount = this->layers;
            else if (this->textureType == TEXTURE_CUBE)
                sliceCount = 6;

            for (int mipmap = 0; mipmap < mipCount; mipmap++)
            {
                for (int slice = 0; slice < sliceCount; slice++)
                {
                    auto* data = this->slices.get(slice, mipmap);

                    if (data != nullptr)
                        this->uploadImageData(data, mipmap, slice, 0, 0);
                }
            }
        }

        bool hasData  = this->slices.get(0, 0) != nullptr;
        int clearMips = 1;

        if (isPixelFormatDepthStencil(this->format))
            clearMips = mipmapCount;

        if (this->isRenderTarget())
        {
        }
        else if (!hasData)
        {
            for (int mipmap = 0; mipmap < clearMips; mipmap++)
            {
            }
        }
    }

    void Texture::setSamplerState(const SamplerState& state)
    {
        this->samplerState = this->validateSamplerState(state);
        gx2.setSamplerState(this, this->samplerState);
    }

    void Texture::uploadByteData(const void* data, size_t size, int level, int slice, const Rect& rect)
    {
        const auto pitch = this->texture->surface.pitch;

        uint8_t* destination = (uint8_t*)this->texture->surface.image;
        uint8_t* source      = (uint8_t*)data;

        size_t pixelSize = getPixelFormatBlockSize(this->format);

        for (uint32_t y = 0; y < rect.h; y++)
        {
            const auto row  = (y * rect.w * pixelSize);
            const auto dest = (rect.x + (y + rect.y) * pitch) * pixelSize;

            std::memcpy(destination + dest, source + row, rect.w * pixelSize);
        }

        const auto imageSize = this->texture->surface.imageSize;
        GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, destination, imageSize);
    }

    void Texture::generateMipmapsInternal()
    {}

    ptrdiff_t Texture::getHandle() const
    {
        return (ptrdiff_t)this->texture;
    }

    ptrdiff_t Texture::getRenderTargetHandle() const
    {
        return (ptrdiff_t)this->target;
    }

    ptrdiff_t Texture::getSamplerHandle() const
    {
        return (ptrdiff_t)(&this->sampler);
    }
} // namespace love
