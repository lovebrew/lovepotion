#include "driver/display/deko.hpp"

#include "modules/graphics/Texture.hpp"
#include <cstring>

namespace love
{
    static void dkImageRectFromRect(const Rect& rectangle, DkImageRect& out)
    {
        out.x = (uint32_t)rectangle.x;
        out.y = (uint32_t)rectangle.y;
        out.z = 0u;

        out.width  = (uint32_t)rectangle.w;
        out.height = (uint32_t)rectangle.h;
        out.depth  = 1u;
    }

    static void updateTextureObject(dk::Image& image, CMemPool::Handle& memory,
                                    dk::ImageDescriptor& descriptor, const Rect& rect, const void* data,
                                    const size_t size, PixelFormat format)
    {
        DkImageFormat gpuFormat;
        if (!deko3d::getConstant(format, gpuFormat))
            throw love::Exception("Invalid image format.");

        auto& scratchPool = d3d.getMemoryPool(deko3d::MEMORYPOOL_DATA);
        auto tempImageMem = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

        if (!tempImageMem)
            throw love::Exception("Failed to allocate temporary image memory");

        std::memcpy(tempImageMem.getCpuAddr(), data, size);

        auto device      = d3d.getDevice();
        auto tempCmdBuff = dk::CmdBufMaker { device }.create();

        auto tempCmdMem = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);
        tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

        dk::ImageLayout layout {};
        dk::ImageLayoutMaker { device }
            .setFlags(0)
            .setFormat(gpuFormat)
            .setDimensions(rect.w, rect.h)
            .initialize(layout);

        auto& imagePool = d3d.getMemoryPool(deko3d::MEMORYPOOL_IMAGE);
        memory          = imagePool.allocate(layout.getSize(), layout.getAlignment());

        if (!memory)
            throw love::Exception("Failed to allocate CMemPool::Handle");

        image.initialize(layout, memory.getMemBlock(), memory.getOffset());
        descriptor.initialize(image);

        dk::ImageView view { image };

        DkImageRect dkRect {};
        dkImageRectFromRect(rect, dkRect);

        tempCmdBuff.copyBufferToImage({ tempImageMem.getGpuAddr() }, view, dkRect);

        auto imageQueue = d3d.getQueue(deko3d::QUEUE_TYPE_IMAGES);

        imageQueue.submitCommands(tempCmdBuff.finishList());
        imageQueue.waitIdle();

        tempCmdMem.destroy();
        tempImageMem.destroy();
    }

    static void createFramebufferObject()
    {}

    Texture::Texture(GraphicsBase* graphics, const Settings& settings, const Slices* data) :
        TextureBase(graphics, settings, data),
        slices(settings.type),
        image {},
        descriptor {},
        sampler {},
        samplerDescriptor {},
        handle(0),
        memory {}
    {
        if (data != nullptr)
            this->slices = *data;

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
        // const auto& layout = this->image.getLayout();
        // if (layout.getSize() != 0)
        //     return true;

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

            memorySize += getPixelFormatSliceSize(this->format, width, height, false) * slices;
        }

        this->setGraphicsMemorySize(memorySize);

        return true;
    }

    void Texture::unloadVolatile()
    {
        d3d.registerTexture(this, false);

        if (this->memory)
            this->memory.destroy();

        this->setGraphicsMemorySize(0);
    }

    void Texture::createTexture()
    {
        if (!this->isRenderTarget())
        {
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

        d3d.registerTexture(this, true);
        this->setSamplerState(this->samplerState);

        if (this->slices.getMipmapCount() <= 1 && this->getMipmapsMode() != MIPMAPS_NONE)
            this->generateMipmaps();
    }

    void Texture::setSamplerState(const SamplerState& state)
    {
        this->samplerState = this->validateSamplerState(state);
        d3d.setSamplerState(this, this->samplerState);
    }

    void Texture::uploadByteData(const void* data, size_t size, int level, int slice, const Rect& rect)
    {
        updateTextureObject(this->image, this->memory, this->descriptor, rect, data, size, this->format);
    }

    void Texture::generateMipmapsInternal()
    {}

    ptrdiff_t Texture::getHandle() const
    {
        return (ptrdiff_t)this->handle;
    }

    ptrdiff_t Texture::getRenderTargetHandle() const
    {
        return (ptrdiff_t)this->handle;
    }

    ptrdiff_t Texture::getSamplerHandle() const
    {
        return (ptrdiff_t)std::addressof(this->sampler);
    }
} // namespace love
