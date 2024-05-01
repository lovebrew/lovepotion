#include "modules/graphics/Texture.hpp"
#include "modules/graphics/Graphics.hpp"

#include "driver/display/Renderer.hpp"
#include "driver/graphics/DrawCommand.hpp"

namespace love
{
    static void createFramebufferObject(C3D_RenderTarget*& target, C3D_Tex* texture, uint16_t width,
                                        uint16_t height, bool clear)
    {
        texture = new C3D_Tex();

        if (!C3D_TexInitVRAM(texture, width, height, GPU_RGBA8))
            throw love::Exception("Failed to create framebuffer texture!");

        target = C3D_RenderTargetCreateFromTex(texture, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH16);
    }

    static void createTextureObject(C3D_Tex*& texture, PixelFormat format, uint16_t width, uint16_t height,
                                    bool clear)
    {
        GPU_TEXCOLOR gpuFormat;
        if (!love::Renderer::getConstant(format, gpuFormat))
            throw love::Exception("Invalid GPU texture format: {:s}.", love::getConstant(format));

        texture = new C3D_Tex();

        if (!C3D_TexInit(texture, width, height, gpuFormat))
            throw love::Exception("Failed to create texture object!");
    }

    Texture::Texture(Graphics& graphics, const Settings& settings, const Slices* data) :
        TextureBase(graphics, settings, data),
        slices(settings.type)
    {
        this->validateDimensions(true);

        this->validatePixelFormat(graphics);

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

        if (this->isRenderTarget())
            createFramebufferObject(this->target, this->texture, powTwoWidth, powTwoHeight, clear);
        else
        {
            createTextureObject(this->texture, this->format, powTwoWidth, powTwoHeight, clear);

            const auto size = love::getPixelFormatSliceSize(this->format, powTwoWidth, powTwoHeight, false);

            if (!hasData)
                std::memset(this->texture->data, 0, size);
            else
                std::memcpy(this->texture->data, this->slices.get(0, 0)->getData(), size);

            C3D_TexFlush(this->texture);
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

    void Texture::draw(Graphics& graphics, const Matrix4& matrix)
    {
        this->draw(graphics, this->quad, matrix);
    }

    void Texture::draw(Graphics& graphics, Quad* quad, const Matrix4& matrix)
    {
        if (!this->readable)
            throw love::Exception("Textures with non-readable formats cannot be drawn.");

        if (this->renderTarget && graphics.isRenderTargetActive(this))
            throw love::Exception("Cannot render a Texture to itself.");

        const auto& viewport = quad->getViewport();

        Vector2 physicalDim { (float)this->texture->width, (float)this->texture->height };
        Vector2 virtualDim { (float)this->pixelWidth, (float)this->pixelHeight };

        refreshQuad(quad, viewport, virtualDim, physicalDim, this->renderTarget);

        const auto& transform = graphics.getTransform();
        bool is2D             = transform.isAffine2DTransform();

        DrawCommand command {};
        command.format      = CommonFormat::XYf_STf_RGBAf;
        command.indexMode   = TRIANGLEINDEX_QUADS;
        command.vertexCount = 4;
        command.texture     = this;

        auto data = Renderer::getInstance().requestBatchDraw(command);

        Matrix4 translated(transform, matrix);

        if (is2D)
            translated.transformXY(data.stream, quad->getVertexPositions(), 4);

        const auto* texCoords = quad->getTextureCoordinates();

        for (int index = 0; index < 4; index++)
        {
            data.stream[index].s     = texCoords[index].x;
            data.stream[index].t     = texCoords[index].y;
            data.stream[index].color = graphics.getColor();
        }
    }

    void Texture::setSamplerState(const SamplerState& state)
    {
        this->samplerState = this->validateSamplerState(state);
        Renderer::getInstance().setSamplerState(this->texture, this->samplerState);
    }

    bool Texture::validateDimensions(bool throwException) const
    {
        bool success = true;

        int largestSize  = 0;
        const char* name = nullptr;

        const bool widthIsLarge  = ((size_t)this->pixelWidth > LOVE_TEX3DS_MAX);
        const bool heightIsLarge = ((size_t)this->pixelHeight > LOVE_TEX3DS_MAX);

        // clang-format off
        if ((this->textureType == TEXTURE_2D || this->textureType == TEXTURE_2D_ARRAY) && (widthIsLarge || heightIsLarge))
        {
            success = false;
            largestSize = std::max(this->pixelWidth, this->pixelHeight);
            name = this->pixelWidth > this->pixelHeight ? "pixel width" : "pixel height";
        }

        if (throwException && name != nullptr)
            throw love::Exception("Cannot create texture: {:s} of {:d} is too large for this system.", name, largestSize);
        // clang-format on

        return success;
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

        return state;
    }

    void Texture::validatePixelFormat(Graphics& graphics) const
    {
        uint32_t usage = PIXELFORMATUSAGEFLAGS_NONE;

        if (renderTarget)
            usage |= PIXELFORMATUSAGEFLAGS_RENDERTARGET;
        if (readable)
            usage |= PIXELFORMATUSAGEFLAGS_SAMPLE;
        if (computeWrite)
            usage |= PIXELFORMATUSAGEFLAGS_COMPUTEWRITE;

        if (!graphics.isPixelFormatSupported(format, (PixelFormatUsageFlags)usage))
        {
            std::string_view name = "unknown";
            love::getConstant(format, name);

            throw love::Exception("The pixel format '{:s}' is not supported by this system.", name);
        }
    }

    ptrdiff_t Texture::getHandle() const
    {
        return (ptrdiff_t)this->texture;
    }
} // namespace love
