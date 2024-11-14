#include "driver/display/deko.hpp"

#include "modules/graphics/Texture.hpp"
#include <cstring>

namespace love
{
    static void createTextureObject(dk::Image& image, CMemPool::Handle& memory,
                                    dk::ImageDescriptor& descriptor, uint32_t width, uint32_t height,
                                    PixelFormat format)
    {
        DkImageFormat gpuFormat;
        if (!deko3d::getConstant(format, gpuFormat))
            throw love::Exception("Invalid image format.");

        auto device = d3d.getDevice();

        dk::ImageLayout layout {};
        dk::ImageLayoutMaker { device }
            .setFlags(0)
            .setFormat(gpuFormat)
            .setDimensions(width, height)
            .initialize(layout);

        auto& pool = d3d.getMemoryPool(deko3d::MEMORYPOOL_IMAGE);

        memory = pool.allocate(layout.getSize(), layout.getAlignment());

        if (!memory)
            throw love::Exception("Failed to allocate CMemPool::Handle!");

        image.initialize(layout, memory.getMemBlock(), memory.getOffset());
        descriptor.initialize(image);
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

        this->memory.destroy();
        this->setGraphicsMemorySize(0);
    }

    void Texture::createTexture()
    {
        if (!this->isRenderTarget())
        {
            try
            {
                createTextureObject(this->image, this->memory, this->descriptor, this->width, this->height,
                                    this->format);
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
        auto& scratchPool    = d3d.getMemoryPool(deko3d::MEMORYPOOL_DATA);
        auto tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

        if (!tempImageMemory)
            throw love::Exception("Failed to allocate temporary memory.");

        std::memcpy(tempImageMemory.getCpuAddr(), data, size);

        auto device      = d3d.getDevice();
        auto tempCmdBuff = dk::CmdBufMaker { device }.create();

        auto tempCmdMem = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);

        if (!tempCmdMem)
            throw love::Exception("Failed to allocate temporary command memory.");

        tempCmdBuff.addMemory(tempCmdMem.getMemBlock(), tempCmdMem.getOffset(), tempCmdMem.getSize());

        dk::ImageView view { this->image };
        DkImageRect dkRect { rect.x, rect.y, 0u, rect.w, rect.h, 1u };

        tempCmdBuff.copyBufferToImage({ tempImageMemory.getGpuAddr() }, view, dkRect);

        auto transferQueue = d3d.getQueue(deko3d::QUEUE_TYPE_IMAGES);

        transferQueue.submitCommands(tempCmdBuff.finishList());
        transferQueue.waitIdle();

        tempCmdMem.destroy();
        tempImageMemory.destroy();
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
