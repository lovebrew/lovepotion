#include "modules/graphics/Texture.tcc"
#include "modules/graphics/Graphics.tcc"

#include "common/Console.hpp"

using namespace love;

int TextureBase::textureCount            = 0;
int64_t TextureBase::totalGraphicsMemory = 0;

#define E_INVALID_LAYER_COUNT \
    "Invalid number of image data layers in mipmap level {:d} (expected {:d}, got {:d})"
#define E_MISSING_DATA "Missing image data (slice {:d}, mipmap level {:d})"
#define E_SIDE_INVALID \
    "{:s} of image data (slice {:d}, mipmap level {:d}) is incorrect (expected {:d}, got {:d})"
#define E_ALL_SLICES_AND_MIPMAPS_TYPE "All texture slices and mipmaps must have the same {:s}"

namespace love
{
    // #region Slices

    void TextureBase::Slices::set(int slice, int mipmap, ImageDataBase* imageData)
    {
        if (this->textureType == TEXTURE_VOLUME)
        {
            if (mipmap >= (int)this->data.size())
                this->data.resize(mipmap + 1);

            if (slice >= (int)this->data[mipmap].size())
                this->data[mipmap].resize(slice + 1);

            this->data[mipmap][slice].set(imageData);
        }
        else
        {
            if (slice >= (int)this->data.size())
                this->data.resize(slice + 1);

            if (mipmap >= (int)this->data[slice].size())
                this->data[slice].resize(mipmap + 1);

            this->data[slice][mipmap].set(imageData);
        }
    }

    ImageDataBase* TextureBase::Slices::get(int slice, int mipmap) const
    {
        if (slice < 0 || slice >= this->getSliceCount(mipmap))
            return nullptr;

        if (mipmap < 0 || mipmap >= this->getMipmapCount(slice))
            return nullptr;

        if (this->textureType == TEXTURE_VOLUME)
            return this->data[mipmap][slice].get();
        else
            return this->data[slice][mipmap].get();
    }

    void TextureBase::Slices::add(CompressedImageData* compressedData, int startSlice, int startMipmap,
                                  bool addAllSlices, bool addAllMipmaps)

    {
        int sliceCount = addAllSlices ? compressedData->getSliceCount() : 1;
        int mipCount   = addAllMipmaps ? compressedData->getMipmapCount() : 1;

        for (int mip = 0; mip < mipCount; mip++)
        {
            for (int slice = 0; slice < sliceCount; slice++)
                this->set(startSlice + slice, startMipmap + mip, compressedData->getSlice(slice, mip));
        }
    }

    int TextureBase::Slices::getSliceCount(int mipmap) const
    {
        if (this->textureType == TEXTURE_VOLUME)
        {
            if (mipmap < 0 || mipmap >= (int)this->data.size())
                return 0;

            return (int)this->data[mipmap].size();
        }
        else
            return (int)this->data.size();
    }

    int TextureBase::Slices::getMipmapCount(int slice) const
    {
        if (this->textureType == TEXTURE_VOLUME)
            return (int)this->data.size();
        else
        {
            if (slice < 0 || slice >= (int)this->data.size())
                return 0;

            return this->data[slice].size();
        }
    }

    bool TextureBase::Slices::validate() const
    {
        int sliceCount = this->getSliceCount();
        int mipCount   = this->getMipmapCount(0);

        if (sliceCount == 0 || mipCount == 0)
            throw love::Exception("At least one ImageData or CompressedImageData is required.");

        if (this->textureType == TEXTURE_CUBE && sliceCount != 6)
            throw love::Exception("Cube textures must have exactly 6 sides.");

        auto* firstData = this->get(0, 0);

        int width   = firstData->getWidth();
        int height  = firstData->getHeight();
        auto format = firstData->getFormat();
        bool linear = firstData->isLinear();

        if (this->textureType == TEXTURE_CUBE && width != height)
            throw love::Exception("Cube textures must be square.");

        int mipW      = width;
        int mipH      = height;
        int mipSlices = sliceCount;

        for (int mip = 0; mip < mipCount; mip++)
        {
            if (this->textureType == TEXTURE_VOLUME)
            {
                sliceCount = this->getSliceCount(mip);

                if (sliceCount != mipSlices)
                    throw love::Exception(E_INVALID_LAYER_COUNT, mip + 1, mipSlices, sliceCount);
            }

            for (int slice = 0; slice < sliceCount; slice++)
            {
                auto* slicedata = get(slice, mip);

                if (slicedata == nullptr)
                    throw love::Exception(E_MISSING_DATA, slice + 1, mip + 1);

                int realW = slicedata->getWidth();
                int realH = slicedata->getHeight();

                if (getMipmapCount(slice) != mipCount)
                    throw love::Exception("All texture layers must have the same mipmap count.");

                if (mipW != realW)
                    throw love::Exception(E_SIDE_INVALID, "width", slice + 1, mip + 1, mipW, realW);

                if (mipH != realH)
                    throw love::Exception(E_SIDE_INVALID, "height", slice + 1, mip + 1, mipH, realH);

                if (format != slicedata->getFormat())
                    throw love::Exception(E_ALL_SLICES_AND_MIPMAPS_TYPE, "pixel format.");

                if (linear != slicedata->isLinear())
                    throw love::Exception(E_ALL_SLICES_AND_MIPMAPS_TYPE, "linear setting.");
            }
        }

        mipW = std::max(mipW / 2, 1);
        mipH = std::max(mipH / 2, 1);

        if (this->textureType == TEXTURE_VOLUME)
            mipSlices = std::max(mipSlices / 2, 1);

        return true;
    }

    // #endregion

    TextureBase::TextureBase(GraphicsBase* graphics, const Settings& settings, const Slices* slices) :
        textureType(settings.type),
        format(settings.format),
        renderTarget(settings.renderTarget),
        computeWrite(settings.computeWrite),
        readable(true),
        viewFormats(settings.viewFormats),
        mipmapsMode(settings.mipmaps),
        width(settings.width),
        height(settings.height),
        depth(settings.type == TEXTURE_VOLUME ? settings.layers : 1),
        layers(settings.type == TEXTURE_2D_ARRAY ? settings.layers : 1),
        mipmapCount(1),
        pixelWidth(0),
        pixelHeight(0),
        requestedMSAA(settings.msaa > 1 ? settings.msaa : 1),
        samplerState(),
        graphicsMemorySize(0),
        debugName(settings.debugName),
        rootView { this, 0, 0 },
        parentView { this, 0, 0 }
    {
        const auto& capabilities = graphics->getCapabilities();
        int requestedMipmapCount = settings.mipmapCount;

        if (slices != nullptr && slices->getMipmapCount() > 0 && slices->getSliceCount() > 0)
        {
            this->textureType = slices->getTextureType();

            if (requestedMSAA > 1)
                throw love::Exception("MSAA textures cannot be created from image data.");

            int dataMipmaps = 1;
            if (slices->validate() && slices->getMipmapCount() > 1)
            {
                dataMipmaps = slices->getMipmapCount();

                if (requestedMipmapCount > 0)
                    dataMipmaps = std::min(requestedMipmapCount, dataMipmaps);
                else
                    requestedMipmapCount = dataMipmaps;
            }

            auto* slice = slices->get(0, 0); //< ImageDataBase*

            this->format = slice->getFormat();
            if (love::isGammaCorrect() && !slice->isLinear())
                this->format = love::getLinearPixelFormat(this->format);

            this->pixelWidth  = slice->getWidth();
            this->pixelHeight = slice->getHeight();

            if (this->textureType == TEXTURE_2D_ARRAY)
                this->layers = slices->getSliceCount();
            else if (this->textureType == TEXTURE_VOLUME)
                this->depth = slices->getSliceCount();

            this->width  = (int)(this->pixelWidth / settings.dpiScale + 0.5);
            this->height = (int)(this->pixelHeight / settings.dpiScale + 0.5);

            if (this->isCompressed() && dataMipmaps <= 1)
                this->mipmapsMode = MIPMAPS_NONE;
        }
        else
        {
            if (this->isCompressed())
                throw love::Exception("Compressed textures must be created with initial data.");

            this->pixelWidth  = ((int)(width * settings.dpiScale) + 0.5);
            this->pixelHeight = ((int)(height * settings.dpiScale) + 0.5);
        }

        if (settings.readable.hasValue)
            this->readable = settings.readable.value;
        else
            this->readable = !this->renderTarget || !love::isPixelFormatDepthStencil(this->format);

        this->format = graphics->getSizedFormat(this->format);
        if (!love::isGammaCorrect() || settings.linear)
            format = love::getLinearPixelFormat(format);

        if (this->mipmapsMode == MIPMAPS_AUTO && this->isCompressed())
            this->mipmapsMode = MIPMAPS_MANUAL;

        if (this->mipmapsMode != MIPMAPS_NONE)
        {
            int totalMipmapCount = getTotalMipmapCount(this->width, this->height, this->depth);

            if (requestedMipmapCount > 0)
                this->mipmapCount = std::min(totalMipmapCount, requestedMipmapCount);
            else
                this->mipmapCount = totalMipmapCount;

            // clang-format off
            if (this->mipmapCount != totalMipmapCount && !capabilities.features[GraphicsBase::FEATURE_MIPMAP_RANGE])
                throw love::Exception(E_CUSTOM_MIPMAP_RANGES_NOT_SUPPORTED, totalMipmapCount, this->mipmapCount);
            // clang-format on
        }

        const char* mipmapError = nullptr;
        if (this->mipmapsMode == MIPMAPS_AUTO && !this->supportsGenerateMipmaps(mipmapError))
        {
            std::string_view name = "unknown";
            love::getConstant(this->format, name);
            throw love::Exception(E_AUTO_MIPMAPS_NOT_SUPPORTED, name);
        }

        if (this->pixelWidth <= 0 || this->pixelHeight <= 0 || this->layers <= 0 || this->depth <= 0)
            throw love::Exception("Texture dimensions must be greater than 0.");

        if (this->textureType != TEXTURE_2D && requestedMSAA > 1)
            throw love::Exception("MSAA is only supported for textures with the 2D texture type.");

        if (!this->renderTarget && requestedMSAA > 1)
            throw love::Exception("MSAA is only supported for render target textures.");

        if (this->readable && love::isPixelFormatDepthStencil(this->format) && settings.msaa > 1)
            throw love::Exception("Readable depth/stencil textures with MSAA are not currently supported.");

        if ((!this->readable || settings.msaa > 1) && this->mipmapsMode != MIPMAPS_NONE)
            throw love::Exception("Non-readable and MSAA textures cannot have mipmaps.");

        if (!this->readable && this->textureType != TEXTURE_2D)
            throw love::Exception("Non-readable pixel formats are only supported for 2D texture types.");

        if (this->isCompressed() && this->renderTarget)
            throw love::Exception("Compressed textures cannot be render targets.");

        for (auto viewFormat : this->viewFormats)
        {
            if (getLinearPixelFormat(viewFormat) == getLinearPixelFormat(this->format))
                continue;

            if (isPixelFormatCompressed(this->format) || isPixelFormatCompressed(viewFormat))
                throw love::Exception(E_COMPRESSED_TEXTURES_MISMATCHED_VIEW);

            if (isPixelFormatColor(this->format) != isPixelFormatColor(viewFormat))
                throw love::Exception(E_COLOR_FORMAT_TEXTURES_MISMATCHED_VIEW);

            if (isPixelFormatDepthStencil(viewFormat))
                throw love::Exception(E_DEPTH_STENCIL_TEXTURES_MISMATCHED_VIEW);

            size_t viewBytes = getPixelFormatBlockSize(viewFormat);
            size_t baseBytes = getPixelFormatBlockSize(this->format);

            if (viewBytes != baseBytes)
                throw love::Exception(E_TEXTURE_VIEW_BITS_PER_PIXEL_MISMATCHED);
        }

        this->validatePixelFormat(graphics);

        if (!capabilities.textureTypes[this->textureType])
        {
            std::string_view name = "unknown";
            TextureBase::getConstant(this->textureType, name);
            throw love::Exception("{:s} textures are not supported on this system.", name);
        }

        this->validateDimensions(true);
        this->samplerState = graphics->getDefaultSamplerState();

        if (this->getMipmapCount() == 1)
            this->samplerState.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;

        Quad::Viewport viewport = { 0, 0, (double)this->width, (double)this->height };
        this->quad.set(new Quad(viewport, this->width, this->height), Acquire::NO_RETAIN);

        ++textureCount;
    }

    TextureBase::~TextureBase()
    {
        this->setGraphicsMemorySize(0);

        if (this == rootView.texture)
            --textureCount;

        if (rootView.texture != this && rootView.texture != nullptr)
            rootView.texture->release();

        if (parentView.texture != this && parentView.texture != nullptr)
            parentView.texture->release();
    }

    void TextureBase::setGraphicsMemorySize(int64_t size)
    {
        totalGraphicsMemory = std::max(totalGraphicsMemory - this->graphicsMemorySize, (int64_t)0);

        size                     = std::max(size, (int64_t)0);
        this->graphicsMemorySize = size;
        totalGraphicsMemory += size;
    }

    void TextureBase::draw(GraphicsBase* graphics, const Matrix4& matrix)
    {
        this->draw(graphics, this->quad, matrix);
    }

    static constexpr ShaderBase::StandardShader shader =
        (Console::is(Console::CTR) ? ShaderBase::STANDARD_DEFAULT : ShaderBase::STANDARD_TEXTURE);

    void TextureBase::draw(GraphicsBase* graphics, Quad* quad, const Matrix4& matrix)
    {
        if (!this->readable)
            throw love::Exception("Textures with non-readable formats cannot be drawn.");

        if (this->renderTarget && graphics->isRenderTargetActive(this))
            throw love::Exception("Cannot render a Texture to itself.");

        const auto& transform = graphics->getTransform();
        bool is2D             = transform.isAffine2DTransform();

        BatchedDrawCommand command {};
        command.format      = CommonFormat::XYf_STf_RGBAf;
        command.indexMode   = TRIANGLEINDEX_QUADS;
        command.vertexCount = 4;
        command.texture     = this;
        command.shaderType  = shader;

        BatchedVertexData data = graphics->requestBatchedDraw(command);

        Matrix4 translated(transform, matrix);

        XYf_STf_RGBAf* stream = (XYf_STf_RGBAf*)data.stream;

        if (is2D)
            translated.transformXY(stream, quad->getVertexPositions(), 4);

        if constexpr (Console::is(Console::CTR))
            this->updateQuad(quad);

        const auto* texCoords = quad->getTextureCoordinates();
        Color color           = graphics->getColor();

        for (int index = 0; index < 4; index++)
        {
            stream[index].s     = texCoords[index].x;
            stream[index].t     = texCoords[index].y;
            stream[index].color = color;
        }
    }

    void TextureBase::drawLayer(GraphicsBase* graphics, int layer, const Matrix4& matrix)
    {
        this->drawLayer(graphics, layer, quad, matrix);
    }

    void TextureBase::drawLayer(GraphicsBase* graphics, int layer, Quad* quad, const Matrix4& matrix)
    {
        if (!this->readable)
            throw love::Exception("Textures with non-readable formats cannot be drawn.");

        if (this->renderTarget && graphics->isRenderTargetActive(this, layer))
            throw love::Exception("Cannot render a Texture to itself.");

        if (this->textureType != TEXTURE_2D_ARRAY)
            throw love::Exception("drawLayer can only be used with Array Textures.");

        if (layer < 0 || layer >= this->layers)
            throw love::Exception("Invalid layer: {:d} (Texture has {:d} layers)", layer + 1, this->layers);

        const auto& transform = graphics->getTransform();
        bool is2D             = transform.isAffine2DTransform();

        Matrix4 translated(transform, matrix);

        BatchedDrawCommand command {};
        command.format      = CommonFormat::XYf_STPf_RGBAf;
        command.indexMode   = TRIANGLEINDEX_QUADS;
        command.vertexCount = 4;
        command.texture     = this;
        command.shaderType  = shader;

        auto data             = graphics->requestBatchedDraw(command);
        XYf_STf_RGBAf* stream = (XYf_STf_RGBAf*)data.stream;

        if (is2D)
            translated.transformXY((Vertex*)data.stream, quad->getVertexPositions(), 4);

        if constexpr (Console::is(Console::CTR))
            this->updateQuad(this->quad);

        const Vector2* texCoords = quad->getTextureCoordinates();

        for (int index = 0; index < 4; index++)
        {
            stream[index].s     = texCoords[index].x;
            stream[index].t     = texCoords[index].y;
            stream[index].color = graphics->getColor();
        }
    }

    Quad* TextureBase::getQuad()
    {
        return this->quad;
    }

    void TextureBase::uploadImageData(ImageDataBase* data, int level, int slice, int x, int y)
    {
        Rect rectangle = { x, y, data->getWidth(), data->getHeight() };

        size_t size = data->getSize();
        if constexpr (Console::is(Console::CTR))
            size = getPixelFormatSliceSize(this->format, rectangle.w, rectangle.h, true);

        this->uploadByteData(data->getData(), size, level, slice, rectangle);
    }

    void TextureBase::replacePixels(ImageDataBase* data, int slice, int mipmap, int x, int y,
                                    bool reloadMipmaps)
    {
        if (!this->isReadable())
            throw love::Exception("replacePixels can only be called on a readable Texture.");

        if (this->getMSAA() > 1)
            throw love::Exception("replacePixels cannot be called on a multisampled Texture.");

        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
        if (graphics != nullptr && graphics->isRenderTargetActive(this))
            throw love::Exception("Cannot replace pixels of a Texture that is currently being rendered to.");

        if (this->getHandle() == 0)
            return;

        if (getLinearPixelFormat(data->getFormat()) != getLinearPixelFormat(this->getPixelFormat()))
            throw love::Exception("Pixel formats must match.");

        if (mipmap < 0 || mipmap >= this->getMipmapCount())
            throw love::Exception("Invalid texture mipmap index {:d}.", mipmap);

        if (slice < 0 || (this->textureType == TEXTURE_CUBE && slice >= 6) ||
            (this->textureType == TEXTURE_VOLUME && slice >= this->getDepth(mipmap)) ||
            (this->textureType == TEXTURE_2D_ARRAY && slice >= this->getLayerCount()))
        {
            throw love::Exception("Invalid slice index {:d}.", slice);
        }

        Rect rect = { x, y, data->getWidth(), data->getHeight() };

        int mipW = this->getPixelWidth(mipmap);
        int mipH = this->getPixelHeight(mipmap);

        const bool rectSizeInvalid = rect.x < 0 || rect.y < 0 || rect.w <= 0 || rect.h <= 0;
        if (rectSizeInvalid || (rect.x + rect.w) > mipW || (rect.y + rect.h) > mipH)
            throw love::Exception(E_INVALID_RECT_DIMENSIONS, rect.x, rect.y, rect.w, rect.h, mipW, mipH);

        GraphicsBase::flushBatchedDrawsGlobal();

        this->uploadImageData(data, mipmap, slice, x, y);

        if (reloadMipmaps && mipmap == 0 && this->getMipmapCount() > 1)
            this->generateMipmaps();
    }

    void TextureBase::replacePixels(const void* data, size_t size, int slice, int mipmap, const Rect& rect,
                                    bool reloadMipmaps)
    {
        if (!this->isReadable() || this->getMSAA() > 1)
            return;

        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);

        if (graphics != nullptr && graphics->isRenderTargetActive(this))
            return;

        GraphicsBase::flushBatchedDrawsGlobal();

        this->uploadByteData(data, size, mipmap, slice, rect);

        if (reloadMipmaps && mipmap == 0 && this->getMipmapCount() > 1)
            this->generateMipmaps();
    }

    SamplerState TextureBase::validateSamplerState(SamplerState state) const
    {
        if (this->readable)
            return state;

        if (state.depthSampleMode.hasValue && !love::isPixelFormatDepth(this->format))
            throw love::Exception("Only depth textures can have a depth sample compare mode.");

        if (state.mipmapFilter != SamplerState::MIPMAP_FILTER_NONE && this->getMipmapCount() == 1)
            state.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;

        if (this->textureType == TEXTURE_CUBE)
            state.wrapU = state.wrapV = state.wrapW = SamplerState::WRAP_CLAMP;

        if (state.minFilter == SamplerState::FILTER_LINEAR ||
            state.magFilter == SamplerState::FILTER_LINEAR ||
            state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        {
            auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
            if (!graphics->isPixelFormatSupported(this->format, PIXELFORMATUSAGE_LINEAR))
            {
                state.minFilter = state.magFilter = SamplerState::FILTER_NEAREST;
                if (state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
                    state.mipmapFilter = SamplerState::MIPMAP_FILTER_NEAREST;
            }
        }

        GraphicsBase::flushBatchedDrawsGlobal();

        return state;
    }

    bool TextureBase::supportsGenerateMipmaps(const char*& outReason) const
    {
        // clang-format off
        if (this->getMipmapsMode() == MIPMAPS_NONE)
        {
            outReason = "generateMipmaps can only be called on a Texture which was created with mipmaps enabled.";
            return false;
        }

        if (isPixelFormatCompressed(this->format))
        {
            outReason = "generateMipmaps cannot be called on a compressed Texture.";
            return false;
        }

        if (isPixelFormatDepthStencil(this->format))
        {
            outReason = "generateMipmaps cannot be called on a depth/stencil Texture.";
            return false;
        }

        if (isPixelFormatInteger(this->format))
        {
            outReason = "generateMipmaps cannot be called on an integer Texture.";
            return false;
        }

        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
        if (graphics != nullptr && !graphics->isPixelFormatSupported(format, PIXELFORMATUSAGE_LINEAR))
        {
            outReason = "generateMipmaps cannot be called on textures with formats that don't support linear filtering on this system.";
            return false;
        }
        // clang-format on

        return true;
    }

    void TextureBase::generateMipmaps()
    {
        const char* error = nullptr;
        if (!this->supportsGenerateMipmaps(error))
            throw love::Exception("{:s}", error);

        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);
        if (graphics != nullptr && graphics->isRenderTargetActive(this))
            throw love::Exception(E_CANNOT_GENERATE_MIPMAPS_RT);

        this->generateMipmapsInternal();
    }

    bool TextureBase::validateDimensions(bool throwException) const
    {
        bool success = true;

        auto* graphics = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);

        if (graphics == nullptr)
            return false;

        const auto& capabilities = graphics->getCapabilities();

        int max2DSize   = capabilities.limits[GraphicsBase::LIMIT_TEXTURE_SIZE];
        int max3DSize   = capabilities.limits[GraphicsBase::LIMIT_VOLUME_TEXTURE_SIZE];
        int maxCubeSize = capabilities.limits[GraphicsBase::LIMIT_CUBE_TEXTURE_SIZE];
        int maxLayers   = capabilities.limits[GraphicsBase::LIMIT_TEXTURE_LAYERS];

        int largestDimension    = 0;
        const char* largestName = nullptr;

        if ((this->textureType == TEXTURE_2D || this->textureType == TEXTURE_2D_ARRAY) &&
            (this->pixelWidth > max2DSize || pixelHeight > max2DSize))
        {
            success          = false;
            largestDimension = std::max(this->pixelWidth, this->pixelHeight);
            largestName      = this->pixelWidth > this->pixelHeight ? "pixel width" : "pixel height";
        }
        else if (this->textureType == TEXTURE_2D_ARRAY && layers > maxLayers)
        {
            success          = false;
            largestDimension = layers;
            largestName      = "array layer count";
        }
        else if (this->textureType == TEXTURE_CUBE &&
                 (this->pixelWidth > maxCubeSize || this->pixelWidth != this->pixelHeight))
        {
            success          = false;
            largestDimension = std::max(this->pixelWidth, this->pixelHeight);
            largestName      = this->pixelWidth > this->pixelHeight ? "pixel width" : "pixel height";

            if (throwException && this->pixelWidth != this->pixelHeight)
                throw love::Exception("Cubemap textures must have equal width and height.");
        }
        else if (this->textureType == TEXTURE_VOLUME &&
                 (this->pixelWidth > max3DSize || this->pixelHeight > max3DSize || this->depth > max3DSize))
        {
            success          = false;
            largestDimension = std::max(this->pixelWidth, std::max(this->pixelHeight, this->depth));
            if (largestDimension == this->pixelWidth)
                largestName = "pixel width";
            else if (largestDimension == this->pixelHeight)
                largestName = "pixel height";
            else
                largestName = "pixel depth";
        }

        if (throwException && largestName != nullptr)
            throw love::Exception(E_CANNOT_CREATE_TEXTURE, largestName, largestDimension);

        return success;
    }

    void TextureBase::validatePixelFormat(GraphicsBase* graphics) const
    {
        uint32_t usage = PIXELFORMATUSAGEFLAGS_NONE;

        if (this->renderTarget)
            usage |= PIXELFORMATUSAGEFLAGS_RENDERTARGET;

        if (this->readable)
            usage |= PIXELFORMATUSAGEFLAGS_SAMPLE;

        if (this->computeWrite)
            usage |= PIXELFORMATUSAGEFLAGS_COMPUTEWRITE;

        if (!graphics->isPixelFormatSupported(this->format, usage))
        {
            std::string_view name = "unknown";
            love::getConstant(this->format, name);

            std::string_view readableString = "";
            if (this->readable != !isPixelFormatDepthStencil(this->format))
                readableString = this->readable ? "readable" : "non-readable";

            std::string_view rtargetStr = "";
            if (this->computeWrite)
                rtargetStr = "as a compute shader-writable texture";
            else if (this->renderTarget)
                rtargetStr = "as a render target";

            throw love::Exception(E_TEXTURE_PIXELFORMAT_NOT_SUPPORTED, name, readableString, rtargetStr);
        }
    }

    int TextureBase::getSliceCount(int mipmap) const
    {
        switch (this->textureType)
        {
            case TEXTURE_2D:
                return 1;
            case TEXTURE_VOLUME:
                return this->getDepth(mipmap);
            case TEXTURE_2D_ARRAY:
                return this->layers;
            case TEXTURE_CUBE:
                return 6;
            default:
                break;
        }

        return 1;
    }
} // namespace love
