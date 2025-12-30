#include "modules/graphics/GraphicsReadback.hpp"
#include "modules/data/ByteData.hpp"
#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Texture.tcc"
#include "modules/image/Image.hpp"
#include "modules/image/ImageData.hpp"

#define E_GRAPHICS_READBACK_FROM_ACTIVE_RENDERTARGET \
    "readbackTexture cannot be called while that Texture is an active render target."
#define E_ASYNC_NONRENDERTARGET_UNSUPPORTED \
    "readbackTextureAsync with a non-render-target texture is not supported on this system."
#define E_NONRENDERTARGET_UNSUPPORTED \
    "readbackTexture with a non-render-target texture is not supported on this system."
#define E_MISMATCH_TEXTURE_FORMAT \
    "Destination ImageData pixel format does not match the source Texture's pixel format."
#define E_RECTANGLE_DIMENSIONS_INVALID \
    "The specified rectangle does not fit within the destination ImageData's dimensions."

namespace love
{
    Type GraphicsReadbackBase::type("GraphicsReadback", &Object::type);

    GraphicsReadbackBase::GraphicsReadbackBase(GraphicsBase* graphics, ReadbackMethod method,
                                               TextureBase* texture, int slice, int mipmap, const Rect& rect,
                                               ImageData* destination, int destinationX, int destinationY) :
        dataType(DATA_TEXTURE),
        method(method),
        imageData(destination),
        rect(rect)
    {
        const auto& capabilities = graphics->getCapabilities();

        if (graphics->isRenderTargetActive(texture))
            throw love::Exception(E_GRAPHICS_READBACK_FROM_ACTIVE_RENDERTARGET);

        if (!texture->isReadable())
            throw love::Exception("readbackTexture requires a readable Texture.");

        const auto width  = texture->getPixelWidth(mipmap);
        const auto height = texture->getPixelHeight(mipmap);
        auto textureType  = texture->getTextureType();

        if (rect.x < 0 || rect.y < 0 || rect.w <= 0 || rect.h <= 0 || (rect.x + rect.w) > width ||
            (rect.y + rect.h) > height)
        {
            throw love::Exception("Invalid rectangle dimensions.");
        }

        if (slice < 0 || (textureType == TEXTURE_VOLUME && slice >= texture->getDepth(mipmap)) ||
            (textureType == TEXTURE_2D_ARRAY && slice >= texture->getLayerCount()) ||
            (textureType == TEXTURE_CUBE && slice >= 6))
        {
            throw love::Exception("Invalid slice index.");
        }

        this->textureFormat  = getLinearPixelFormat(texture->getPixelFormat());
        this->isFormatLinear = isGammaCorrect() && !isPixelFormatSRGB(texture->getPixelFormat());

        if (!ImageData::validPixelFormat(this->textureFormat))
        {
            std::string_view formatname = "unknown";
            love::getConstant(textureFormat, formatname);
            throw love::Exception("ImageData with the {:s} pixel format is not supported.", formatname);
        }

        bool isRenderTarget = texture->isRenderTarget();
        if (method == READBACK_ASYNC)
        {
            if (!isRenderTarget && !capabilities.features[GraphicsBase::FEATURE_COPY_TEXTURE_TO_BUFFER])
                throw love::Exception(E_ASYNC_NONRENDERTARGET_UNSUPPORTED);
        }
        else
        {
            if (!isRenderTarget && !capabilities.features[GraphicsBase::FEATURE_COPY_TEXTURE_TO_BUFFER])
                throw love::Exception(E_NONRENDERTARGET_UNSUPPORTED);
        }

        if (destination != nullptr)
        {
            if (getLinearPixelFormat(destination->getFormat()) != this->textureFormat)
                throw love::Exception(E_MISMATCH_TEXTURE_FORMAT);

            if (destinationX < 0 || destinationY < 0)
                throw love::Exception("Invalid destination ImageData x/y coordinates.");

            if (destinationX + rect.w > destination->getWidth() ||
                destinationY + rect.h > destination->getHeight())
            {
                throw love::Exception(E_RECTANGLE_DIMENSIONS_INVALID);
            }
        }

        this->imageDataX = destination != nullptr ? destinationX : 0;
        this->imageDataY = destination != nullptr ? destinationY : 0;
    }

    GraphicsReadbackBase::~GraphicsReadbackBase()
    {}

    ByteData* GraphicsReadbackBase::getBufferData() const
    {
        if (!this->isComplete())
            return nullptr;

        return this->bufferData;
    }

    ImageData* GraphicsReadbackBase::getImageData() const
    {
        if (!this->isComplete())
            return nullptr;

        return this->imageData;
    }

    void* GraphicsReadbackBase::prepareReadbackDestination(size_t size)
    {
        if (this->dataType == DATA_TEXTURE)
        {
            if (this->imageData.get())
            {
                int pixels    = this->imageDataY * this->imageData->getWidth() + this->imageDataX;
                size_t offset = getPixelFormatUncompressedRowSize(this->textureFormat, pixels);

                return (uint8_t*)this->imageData->getData() + offset;
            }
            else
            {
                auto* module = Module::getInstance<Image>(Module::M_IMAGE);
                if (module == nullptr)
                    throw love::Exception("The love.image module must be loaded for readbackTexture.");

                this->imageData.set(module->newImageData(rect.w, rect.w, this->textureFormat, nullptr),
                                    Acquire::NO_RETAIN);
                this->imageData->setLinear(this->isFormatLinear);
                return this->imageData->getData();
            }
        }
        else
        {
            if (!this->bufferData.get())
                this->bufferData.set(new ByteData(size, false), Acquire::NO_RETAIN);

            return (uint8_t*)this->bufferData->getData() + this->bufferDataOffset;
        }
    }

    GraphicsReadbackBase::Status GraphicsReadbackBase::readbackBuffer(Buffer* buffer, size_t offset,
                                                                      size_t size)
    {
        return STATUS_ERROR;
    }
} // namespace love
