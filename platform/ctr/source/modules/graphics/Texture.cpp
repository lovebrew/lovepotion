#include "modules/graphics/Texture.hpp"
#include "modules/graphics/Graphics.hpp"

#include "driver/display/Renderer.hpp"
#include "driver/graphics/DrawCommand.hpp"

namespace love
{
    static void createFramebufferObject(C3D_RenderTarget*& target, C3D_Tex* texture, uint16_t width,
                                        uint16_t height)
    {
        texture = new C3D_Tex();

        if (!C3D_TexInitVRAM(texture, width, height, GPU_RGBA8))
            throw love::Exception("Failed to create framebuffer texture!");

        target = C3D_RenderTargetCreateFromTex(texture, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH16);
    }

    static void createTextureObject(C3D_Tex*& texture, PixelFormat format, uint16_t width, uint16_t height)
    {
        GPU_TEXCOLOR gpuFormat;
        if (!love::Renderer::getConstant(format, gpuFormat))
            throw love::Exception("Invalid GPU texture format: {:s}.", love::getConstant(format));

        texture = new C3D_Tex();

        if (!C3D_TexInit(texture, width, height, gpuFormat))
            throw love::Exception("Failed to create texture object!");
    }

    Texture::Texture(GraphicsBase* graphics, const Settings& settings, const Slices* data) :
        TextureBase(graphics, settings, data),
        slices(settings.type)
    {
        // this->validateDimensions(true);

        // this->validatePixelFormat(graphics);

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

    void Texture::createTexture()
    {
        const bool hasData = this->slices.get(0, 0) != nullptr;
        const bool clear   = !hasData;

        const auto powTwoWidth  = NextPo2(this->pixelWidth);
        const auto powTwoHeight = NextPo2(this->pixelHeight);

        try
        {
            if (this->isRenderTarget())
                createFramebufferObject(this->target, this->texture, powTwoWidth, powTwoHeight);
            else
            {
                createTextureObject(this->texture, this->format, powTwoWidth, powTwoHeight);
                auto size = love::getPixelFormatSliceSize(this->format, powTwoWidth, powTwoHeight, false);

                if (hasData)
                    std::memcpy(this->texture->data, this->slices.get(0, 0)->getData(), size);

                C3D_TexFlush(this->texture);
            }
        }
        catch (love::Exception& e)
        {
            throw;
        }

        this->setSamplerState(this->samplerState);
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

        if (!isRenderTarget)
        {
            for (size_t index = 0; index < 4; index++)
                quad->setTextureCoordinate(index, { textureCoords[index].x, 1.0f - textureCoords[index].y });

            return;
        }

        quad->setTextureCoordinate(0, getVertex(0.0f, 0.0f, virtualSize, realSize));
        quad->setTextureCoordinate(1, getVertex(0.0f, virtualSize.y, virtualSize, realSize));
        quad->setTextureCoordinate(2, getVertex(virtualSize.x, virtualSize.y, virtualSize, realSize));
        quad->setTextureCoordinate(3, getVertex(virtualSize.x, 0.0f, virtualSize, realSize));
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
        Renderer::getInstance().setSamplerState(this->texture, this->samplerState);
    }

    SamplerState Texture::validateSamplerState(SamplerState state) const
    {
        if (this->readable)
            return state;

        if (state.depthSampleMode.hasValue && !love::isPixelFormatDepth(this->format))
            throw love::Exception("Only depth textures can have a depth sample compare mode.");

        if (state.mipmapFilter != SamplerState::MIPMAP_FILTER_NONE && this->getMipmapCount() == 1)
            state.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;

        if (this->textureType == TEXTURE_CUBE)
            state.wrapU = state.wrapV = state.wrapW = SamplerState::WRAP_CLAMP;

        Graphics::flushBatchedDrawsGlobal();

        return state;
    }

    void Texture::replacePixels(ImageDataBase* data, int slice, int mipmap, int x, int y, bool reloadMipmaps)
    {
        if (!this->isReadable())
            throw love::Exception("replacePixels can only be called on a readable Texture.");

        if (this->getMSAA() > 1)
            throw love::Exception("replacePixels cannot be called on a multisampled Texture.");

        auto* graphics = Module::getInstance<Graphics>(Module::M_GRAPHICS);
        if (graphics == nullptr && graphics->isRenderTargetActive(this))
            throw love::Exception("Cannot replace pixels of a Texture that is currently being rendered to.");

        if (this->getHandle() == 0)
            return;

        if (data->getFormat() != this->getPixelFormat())
            throw love::Exception("Image data format does not match Texture format.");

        if (mipmap < 0 || mipmap >= this->getMipmapCount())
            throw love::Exception("Invalid mipmap level: {:d}.", mipmap);

        if (slice < 0 || (this->textureType == TEXTURE_CUBE && slice >= 6) ||
            (this->textureType == TEXTURE_VOLUME && slice >= this->getDepth(mipmap)) ||
            (this->textureType == TEXTURE_2D_ARRAY && slice >= this->getLayerCount()))
        {
            throw love::Exception("Invalid slice: {:d}.", slice);
        }

        Rect rectangle = { x, y, data->getWidth(), data->getHeight() };

        int mipmapWidth  = this->getPixelWidth(mipmap);
        int mipmapHeight = this->getPixelHeight(mipmap);

        if (rectangle.x < 0 || rectangle.y < 0 || rectangle.w <= 0 || rectangle.h <= 0 ||
            (rectangle.x + rectangle.w) > mipmapWidth || (rectangle.y + rectangle.h) > mipmapHeight)
        {
            throw love::Exception("Invalid rectangle dimensions (x: {:d}, y: {:d}, w: {:d}, h: {:d}).",
                                  rectangle.x, rectangle.y, rectangle.w, rectangle.h);
        }

        Graphics::flushBatchedDrawsGlobal();

        this->replacePixels(data->getData(), data->getSize(), slice, mipmap, rectangle, reloadMipmaps);
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

    void Texture::replacePixels(const void* data, size_t size, int slice, int mipmap, const Rect& rect,
                                bool reloadMipmaps)
    {
        // clang-format off
        switch (this->getPixelFormat())
        {
            case PIXELFORMAT_RGB565_UNORM:
                replaceTiledPixels<uint16_t>(data, this->texture->data, rect, this->texture->width, this->texture->height);
                break;
            case PIXELFORMAT_RGBA8_UNORM:
            default:
                replaceTiledPixels<uint32_t>(data, this->texture->data, rect, this->texture->width, this->texture->height);
                break;
        }
        // clang-format on

        C3D_TexFlush(this->texture);

        Graphics::flushBatchedDrawsGlobal();
    }

    // void Texture::validatePixelFormat(Graphics& graphics) const
    // {
    //     uint32_t usage = PIXELFORMATUSAGEFLAGS_NONE;

    //     if (renderTarget)
    //         usage |= PIXELFORMATUSAGEFLAGS_RENDERTARGET;
    //     if (readable)
    //         usage |= PIXELFORMATUSAGEFLAGS_SAMPLE;
    //     if (computeWrite)
    //         usage |= PIXELFORMATUSAGEFLAGS_COMPUTEWRITE;

    //     if (!graphics.isPixelFormatSupported(format, (PixelFormatUsageFlags)usage))
    //     {
    //         std::string_view name = "unknown";
    //         love::getConstant(format, name);

    //         throw love::Exception("The pixel format '{:s}' is not supported by this system.", name);
    //     }
    // }

    // bool Texture::validateDimensions(bool throwException) const
    // {
    //     bool success = true;

    //     int largestSize  = 0;
    //     const char* name = nullptr;

    //     const bool widthIsLarge  = ((size_t)this->pixelWidth > LOVE_TEX3DS_MAX);
    //     const bool heightIsLarge = ((size_t)this->pixelHeight > LOVE_TEX3DS_MAX);

    //     // clang-format off
    //     if ((this->textureType == TEXTURE_2D || this->textureType == TEXTURE_2D_ARRAY) && (widthIsLarge ||
    //     heightIsLarge))
    //     {
    //         success = false;
    //         largestSize = std::max(this->pixelWidth, this->pixelHeight);
    //         name = this->pixelWidth > this->pixelHeight ? "pixel width" : "pixel height";
    //     }

    //     if (throwException && name != nullptr)
    //         throw love::Exception("Cannot create texture: {:s} of {:d} is too large for this system.",
    //         name, largestSize);
    //     // clang-format on

    //     return success;
    // }

    ptrdiff_t Texture::getHandle() const
    {
        return (ptrdiff_t)this->texture;
    }
} // namespace love
