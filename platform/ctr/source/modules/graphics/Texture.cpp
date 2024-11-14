#include "modules/graphics/Texture.hpp"
#include "modules/graphics/Graphics.hpp"

#include "driver/display/citro3d.hpp"
#include "driver/graphics/DrawCommand.hpp"

namespace love
{
    static void createFramebufferObject(C3D_RenderTarget*& target, C3D_Tex* texture, uint16_t width,
                                        uint16_t height, bool clear)
    {
        texture = new C3D_Tex();

        if (!C3D_TexInitVRAM(texture, width, height, GPU_RGBA8))
            throw love::Exception("Failed to create framebuffer texture!");

        auto* previousFramebuffer = c3d.getFramebuffer();

        target = C3D_RenderTargetCreateFromTex(texture, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH16);

        c3d.bindFramebuffer(target);

        if (clear)
            c3d.clear({ 0, 0, 0, 0 });

        c3d.bindFramebuffer(previousFramebuffer);
    }

    static void createTextureObject(C3D_Tex*& texture, PixelFormat format, uint16_t width, uint16_t height)
    {
        GPU_TEXCOLOR gpuFormat;
        if (!citro3d::getConstant(format, gpuFormat))
            throw love::Exception("Invalid GPU texture format: {:s}.", love::getConstant(format));

        texture = new C3D_Tex();

        if (!C3D_TexInit(texture, width, height, gpuFormat))
            throw love::Exception("Failed to create texture object!");
    }

    Texture::Texture(GraphicsBase* graphics, const Settings& settings, const Slices* data) :
        TextureBase(graphics, settings, data),
        slices(settings.type)
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
        {
            C3D_TexDelete(this->texture);
            delete this->texture;
        }

        if (this->target != nullptr)
        {
            C3D_RenderTargetDelete(this->target);
            C3D_TexDelete(this->texture);

            delete this->texture;
        }

        this->setGraphicsMemorySize(0);
    }

    static void* getTextureFace(C3D_Tex* texture, GPU_TEXFACE face, int mipmap, uint32_t* size = nullptr)
    {
        if (C3D_TexGetType(texture) == GPU_TEX_CUBE_MAP)
            return C3D_TexCubeGetImagePtr(texture, face, mipmap, size);

        return C3D_Tex2DGetImagePtr(texture, mipmap, size);
    }

    void Texture::createTexture()
    {
        const auto powTwoWidth  = NextPo2(this->pixelWidth);
        const auto powTwoHeight = NextPo2(this->pixelHeight);

        /*
        ** Textures cannot be initialized in VRAM unless they are render targets.
        */
        if (!this->isRenderTarget())
        {
            try
            {
                createTextureObject(this->texture, this->format, powTwoWidth, powTwoHeight);
            }
            catch (love::Exception&)
            {
                throw;
            }

            int mipCount   = this->getMipmapCount();
            int sliceCount = 1;

            if (this->textureType == TEXTURE_VOLUME)
                sliceCount = getDepth();
            else if (this->textureType == TEXTURE_2D_ARRAY)
                sliceCount = getLayerCount();
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

        bool hasData = this->slices.get(0, 0) != nullptr;

        int clearMips = 1;
        if (isPixelFormatDepthStencil(this->format))
            clearMips = mipmapCount;

        if (this->isRenderTarget())
        {
            bool clear = !hasData;

            try
            {
                createFramebufferObject(this->target, this->texture, powTwoWidth, powTwoHeight, clear);
            }
            catch (love::Exception&)
            {
                throw;
            }
        }
        else if (!hasData)
        {
            for (int mipmap = 0; mipmap < clearMips; mipmap++)
            {
                uint32_t mipmapSize = 0;

                GPU_TEXFACE face = GPU_TEXFACE_2D;
                if (this->textureType == TEXTURE_CUBE)
                    face = GPU_TEXFACE(GPU_POSITIVE_X + mipmap);

                auto* data = getTextureFace(this->texture, face, mipmap, &mipmapSize);

                std::memset(data, 0, mipmapSize);
            }
        }

        this->setSamplerState(this->samplerState);

        if (this->slices.getMipmapCount() <= 1 && this->getMipmapsMode() != MIPMAPS_NONE)
            this->generateMipmaps();
    }

    static Vector2 getVertex(const float x, const float y, const Vector2& virtualDim,
                             const Vector2& physicalDim)
    {
        const auto u = x / physicalDim.x;
        const auto v = (virtualDim.y - y) / physicalDim.y;

        return Vector2(u, v);
    }

    static void refreshQuad(Quad* quad, const Quad::Viewport& viewport, const Vector2& virtualSize,
                            const Vector2& realSize, bool isRenderTarget)
    {
        quad->refresh(viewport, realSize.x, realSize.y);
        const auto* textureCoords = quad->getTextureCoordinates();

        if (isRenderTarget)
        {
            quad->setTextureCoordinate(0, getVertex(0.0f, 0.0f, virtualSize, realSize));
            quad->setTextureCoordinate(1, getVertex(0.0f, virtualSize.y, virtualSize, realSize));
            quad->setTextureCoordinate(2, getVertex(virtualSize.x, virtualSize.y, virtualSize, realSize));
            quad->setTextureCoordinate(3, getVertex(virtualSize.x, 0.0f, virtualSize, realSize));

            return;
        }

        for (size_t index = 0; index < 4; index++)
            quad->setTextureCoordinate(index, { textureCoords[index].x, 1.0f - textureCoords[index].y });
    }

    void Texture::updateQuad(Quad* quad)
    {
        const auto& viewport = quad->getViewport();

        Vector2 physicalDim { (float)this->texture->width, (float)this->texture->height };
        Vector2 virtualDim { (float)this->pixelWidth, (float)this->pixelHeight };

        refreshQuad(quad, viewport, virtualDim, physicalDim, this->renderTarget);
    }

    void Texture::setSamplerState(const SamplerState& state)
    {
        this->samplerState = this->validateSamplerState(state);
        c3d.setSamplerState(this->texture, this->samplerState);
    }

    template<typename T>
    void replaceTiledPixels(const void* source, void* texture, const Rect& rect, const int width,
                            const int height)
    {
        const int sourcePowerTwo = NextPo2(rect.w);
        const int destPowerTwo   = NextPo2(width);

        for (int y = 0; y < std::min(rect.h, height - rect.y); y++)
        {
            for (int x = 0; x < std::min(rect.w, width - rect.x); x++)
            {
                const auto* sourcePixel = Color::fromTile<T>(source, sourcePowerTwo, x, y);
                auto* destPixel         = Color::fromTile<T>(texture, destPowerTwo, rect.x + x, rect.y + y);

                *destPixel = *sourcePixel;
            }
        }
    }

    void Texture::uploadByteData(const void* data, size_t size, int level, int slice, const Rect& rect)
    {
        const auto face   = GPU_TEXFACE(slice);
        void* textureData = getTextureFace(this->texture, face, level);

        if (textureData == nullptr)
            throw love::Exception("Failed to get texture data pointer (not initialized?).");

        const auto mipWidth  = this->getPixelWidth(level);
        const auto mipHeight = this->getPixelHeight(level);

        // copy it directly if the size is the whole thing
        if (rect == Rect(0, 0, mipWidth, mipHeight))
            std::memcpy(textureData, data, size);
        else
        {
            switch (this->getPixelFormat())
            {
                case PIXELFORMAT_RGB565_UNORM:
                case PIXELFORMAT_RGB5A1_UNORM:
                    replaceTiledPixels<uint16_t>(data, textureData, rect, this->width, this->height);
                    break;
                case PIXELFORMAT_RGBA4_UNORM:
                    replaceTiledPixels<uint16_t>(data, textureData, rect, this->width, this->height);
                    break;
                case PIXELFORMAT_LA8_UNORM:
                    replaceTiledPixels<uint8_t>(data, textureData, rect, this->width, this->height);
                    break;
                case PIXELFORMAT_RGBA8_UNORM:
                default:
                    replaceTiledPixels<uint32_t>(data, textureData, rect, this->width, this->height);
                    break;
            }
        }

        C3D_TexFlush(this->texture);
    }

    void Texture::generateMipmapsInternal()
    {
        if (C3D_TexGetType(this->texture) == GPU_TEX_2D)
        {
            C3D_TexGenerateMipmap(this->texture, GPU_TEXFACE_2D);
            return;
        }

        for (int face = 0; face < 6; face++)
            C3D_TexGenerateMipmap(this->texture, GPU_TEXFACE(face));
    }

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
        return 0;
    }
} // namespace love
