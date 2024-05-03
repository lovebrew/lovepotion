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
            this->mipmapsMode = MIPMAPS_NONE;

        if (this->mipmapsMode != MIPMAPS_NONE)
        {
            int totalMipmapCount = getTotalMipmapCount(this->width, this->height, this->depth);

            if (requestedMipmapCount > 0)
                this->mipmapCount = std::min(totalMipmapCount, requestedMipmapCount);
            else
                this->mipmapCount = totalMipmapCount;

            if (this->mipmapCount != totalMipmapCount)
                throw love::Exception("Custom mipmap ranges for a texture are not supported.");
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

        this->validateViewFormats();

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

    void TextureBase::draw(GraphicsBase* graphics, Quad* quad, const Matrix4& matrix)
    {
        if (!this->readable)
            throw love::Exception("Textures with non-readable formats cannot be drawn.");

        // if (this->renderTarget && graphics.isRenderTargetActive(this))
        //     throw love::Exception("Cannot render a Texture to itself.");

        const auto& transform = graphics->getTransform();
        bool is2D             = transform.isAffine2DTransform();

        BatchedDrawCommand command {};
        command.format      = CommonFormat::XYf_STf_RGBAf;
        command.indexMode   = TRIANGLEINDEX_QUADS;
        command.vertexCount = 4;
        command.texture     = this;

        auto data = graphics->requestBatchDraw(command);

        Matrix4 translated(transform, matrix);

        XYf_STf_RGBAf* stream = (XYf_STf_RGBAf*)data.stream;

        if (is2D)
            translated.transformXY(stream, quad->getVertexPositions(), 4);

        if constexpr (Console::is(Console::CTR))
            this->updateQuad(quad);

        const auto* texCoords = quad->getTextureCoordinates();

        for (int index = 0; index < 4; index++)
        {
            stream[index].s     = texCoords[index].x;
            stream[index].t     = texCoords[index].y;
            stream[index].color = graphics->getColor();
        }
    }

    void TextureBase::validateViewFormats() const
    {}

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
