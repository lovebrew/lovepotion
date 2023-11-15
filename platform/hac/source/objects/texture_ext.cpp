#include <objects/texture_ext.hpp>

#include <modules/graphics_ext.hpp>

#include <common/exception.hpp>

using namespace love;

static void createFramebufferObject(dk::Image& image, CMemPool::Handle& memory,
                                    dk::ImageDescriptor& descriptor, uint32_t width,
                                    uint32_t height)

{
    auto device = Renderer<Console::HAC>::Instance().GetDevice();

    dk::ImageLayout layout;
    dk::ImageLayoutMaker { device }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(width, height)
        .initialize(layout);

    auto poolId   = Renderer<Console::HAC>::IMAGE;
    auto& scratch = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    memory = scratch.allocate(layout.getSize(), layout.getAlignment());
    image.initialize(layout, memory.getMemBlock(), memory.getOffset());

    dk::ImageView imageView { image };
    descriptor.initialize(imageView);
}

static void dkImageRectFromRect(const Rect& rectangle, DkImageRect& out)
{
    out.x = (uint32_t)rectangle.x;
    out.y = (uint32_t)rectangle.y;
    out.z = (uint32_t)0;

    out.width  = (uint32_t)rectangle.w;
    out.height = (uint32_t)rectangle.h;
    out.depth  = (uint32_t)1;
}

static void createTextureObject(dk::Image& image, CMemPool::Handle& memory,
                                dk::ImageDescriptor& descriptor, const void* data, size_t realSize,
                                PixelFormat format, Rect rectangle)
{
    if (data == nullptr)
        throw love::Exception("No data for Texture.");

    std::optional<DkImageFormat> imageFormat;
    if (!(imageFormat = Renderer<Console::HAC>::pixelFormats.Find(format)))
        throw love::Exception("Invalid image format.");

    auto size = love::GetPixelFormatSliceSize(format, rectangle.w, rectangle.h);

    if (realSize != 0)
        size = realSize;

    if (size <= 0)
        throw love::Exception("Invalid PixelFormat slice size.");

    auto poolId       = Renderer<Console::HAC>::DATA;
    auto& scratchPool = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    auto tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        throw love::Exception("Failed to allocate temporary memory.");

    /* copy the data into the temp image memory */
    std::memcpy(tempImageMemory.getCpuAddr(), data, size);

    auto device     = Renderer<Console::HAC>::Instance().GetDevice();
    auto tempCmdBuf = dk::CmdBufMaker { device }.create();

    /* make some memory for the command buffer */
    auto tempCmdMemory = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);

    const auto& memBlock = tempCmdMemory.getMemBlock();
    const auto offset    = tempCmdMemory.getOffset();
    const auto memSize   = tempCmdMemory.getSize();

    /* add the memory to the command buffer */
    tempCmdBuf.addMemory(memBlock, offset, memSize);

    /* set the image layout */
    dk::ImageLayout layout;
    dk::ImageLayoutMaker { device }
        .setFlags(0)
        .setFormat(*imageFormat)
        .setDimensions(rectangle.w, rectangle.h)
        .initialize(layout);

    poolId          = Renderer<Console::HAC>::IMAGE;
    auto& imagePool = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    memory = imagePool.allocate(layout.getSize(), layout.getAlignment());

    if (!memory)
        throw love::Exception("Failed to allocate image memory handle");

    image.initialize(layout, memory.getMemBlock(), memory.getOffset());
    descriptor.initialize(image);

    dk::ImageView view { image };

    DkImageRect dkRectangle {};
    dkImageRectFromRect(rectangle, dkRectangle);

    tempCmdBuf.copyBufferToImage({ tempImageMemory.getGpuAddr() }, view, dkRectangle);

    const auto queueId = Renderer<Console::HAC>::QUEUE_IMAGES;
    auto transferQueue = Renderer<Console::HAC>::Instance().GetQueue(queueId);

    transferQueue.submitCommands(tempCmdBuf.finishList());
    transferQueue.waitIdle();

    tempCmdMemory.destroy();
    tempImageMemory.destroy();
}

Texture<Console::HAC>::Texture(const Graphics<Console::HAC>* graphics, const Settings& settings,
                               const Slices* data) :
    Texture<Console::ALL>(settings, data),
    textureHandle(0),
    image {},
    descriptor {},
    memory {}
{
    this->format = graphics->GetSizedFormat(format, this->renderTarget, this->readable);

    if (this->mipmapMode == MIPMAPS_AUTO && this->IsCompressed())
        this->mipmapMode = MIPMAPS_MANUAL;

    if (this->mipmapMode != MIPMAPS_NONE)
        this->mipmapCount =
            Texture<>::GetTotalMipmapCount(this->pixelWidth, this->pixelHeight, this->depth);

    bool invalidDimensions = this->pixelWidth <= 0 || this->pixelHeight <= 0;
    if (invalidDimensions || this->layers <= 0 || this->depth <= 0)
        throw love::Exception("Texture dimensions must be greater than zero.");

    if (this->textureType != TEXTURE_2D && this->requestedMSAA > 1)
        throw love::Exception("MSAA is only supported for 2D textures.");

    if (!this->renderTarget && this->requestedMSAA > 1)
        throw love::Exception("MSAA is only supported with render target textures.");

    bool isDepthStencilFormat = love::IsPixelFormatDepthStencil(this->format);
    if (this->readable && isDepthStencilFormat && settings.msaa > 1)
        throw love::Exception("Readable depth/stencil textures with MSAA are not supported.");

    if ((!this->readable || settings.msaa > 1) && this->mipmapMode != MIPMAPS_NONE)
        throw love::Exception("Non-readable and MSAA textures cannot have mipmaps.");

    if (!this->readable && this->textureType != TEXTURE_2D)
        throw love::Exception("Non-readable pixel formats are only supported for 2D textures.");

    if (this->IsCompressed() && this->renderTarget)
        throw love::Exception("Compressed textures cannot be render targets.");

    this->state = graphics->GetDefaultSamplerState();

    if (this->GetMipmapCount() == 1)
        this->state.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;

    Quad::Viewport view { 0, 0, (double)this->width, (double)this->height };
    this->quad.Set(new Quad(view, this->width, this->height), Acquire::NORETAIN);

    ++textureCount;

    if (data != nullptr)
        this->slices = *data;

    this->LoadVolatile();

    this->slices.Clear();
}

Texture<Console::HAC>::~Texture()
{
    this->UnloadVolatile();
}

bool Texture<Console::HAC>::LoadVolatile()
{
    if (this->IsReadable())
        this->CreateTexture();

    int64_t memorySize = 0;

    for (int mipmap = 0; mipmap < this->GetMipmapCount(); mipmap++)
    {
        const auto width  = this->GetPixelWidth(mipmap);
        const auto height = this->GetPixelHeight(mipmap);

        const auto faceCount = this->textureType == TEXTURE_CUBE ? 6 : 1;
        const auto slices    = this->GetDepth(mipmap) * this->layers * faceCount;

        memorySize += love::GetPixelFormatSliceSize(this->format, width, height) * slices;
    }

    this->SetGraphicsMemorySize(memorySize);

    return true;
}

void Texture<Console::HAC>::CreateTexture()
{
    Texture<>::CreateTexture();
    bool hasData = this->slices.Get(0, 0) != nullptr;

    int _width  = this->pixelWidth;
    int _height = this->pixelHeight;

    Rect rectangle { 0, 0, _width, _height };

    if (this->IsRenderTarget())
    {
        bool clear = !hasData;

        createFramebufferObject(this->image, this->memory, this->descriptor, _width, _height);
        Renderer<Console::HAC>::Instance().ClearCanvas(this);
    }
    else
    {
        if (!hasData)
        {
            std::vector<uint8_t> empty(_width * _height, 0);
            createTextureObject(this->image, this->memory, this->descriptor, empty.data(),
                                empty.size(), this->format, rectangle);
        }
        else
        {
            createTextureObject(this->image, this->memory, this->descriptor,
                                this->slices.Get(0, 0)->GetData(),
                                this->slices.Get(0, 0)->GetSize(), this->format, rectangle);
        }
    }

    Renderer<Console::HAC>::Instance().Register(this, this->textureHandle);
    this->SetSamplerState(this->state);
}

void Texture<Console::HAC>::SetSamplerState(const SamplerState& state)
{
    Texture<>::SetSamplerState(state);

    this->state.magFilter = this->state.minFilter = SamplerState::FILTER_NEAREST;

    if (this->state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        this->state.mipmapFilter = SamplerState::MIPMAP_FILTER_NEAREST;

    Renderer<Console::HAC>::Instance().SetSamplerState(this, this->state);
}

void Texture<Console::HAC>::UnloadVolatile()
{
    Renderer<Console::HAC>::Instance().UnRegister(this);
    this->memory.destroy();
}

void Texture<Console::HAC>::ReplacePixels(ImageDataBase* data, int slice, int mipmap, int x, int y,
                                          bool reloadMipmaps)
{
    if (!this->IsReadable())
        throw love::Exception("replacePixels can only be called on readable Textures.");

    if (this->GetMSAA() > 1)
        throw love::Exception("replacePixels cannot be called on an MSAA Texture.");

    auto* graphics = Module::GetInstance<Graphics<Console::CAFE>>(Module::M_GRAPHICS);

    if (graphics != nullptr && graphics->IsRenderTargetActive(this))
        throw love::Exception(
            "replacePixels cannot be called on this Texture while it's an active render target.");

    if (!this->memory)
        return;

    if (data->GetFormat() != this->GetPixelFormat())
        throw love::Exception("Pixel formats must match.");

    if (mipmap < 0 || mipmap >= this->GetMipmapCount())
        throw love::Exception("Invalid texture mipmap index %d.", mipmap + 1);

    const bool isCubeType   = this->textureType == TEXTURE_CUBE;
    const bool isVolumeType = this->textureType == TEXTURE_VOLUME;
    const bool isArrayType  = this->textureType == TEXTURE_2D_ARRAY;

    if (slice < 0 || (isCubeType && slice >= 6) ||
        (isVolumeType && slice >= this->GetDepth(mipmap)) ||
        (isArrayType && slice >= this->GetLayerCount()))
    {
        throw love::Exception("Invalid texture slice index %d", slice + 1);
    }

    Rect rectangle = { x, y, data->GetWidth(), data->GetHeight() };

    int mipWidth  = this->GetPixelWidth(mipmap);
    int mipHeight = this->GetPixelHeight(mipmap);

    if (rectangle.x < 0 || rectangle.y < 0 || rectangle.w <= 0 || rectangle.h <= 0 ||
        (rectangle.x + rectangle.w) > mipWidth || (rectangle.y + rectangle.h) > mipHeight)
    {
        throw love::Exception(
            "Invalid rectangle dimensions (x = %d, y = %d, w = %d, h = %d) for %dx%d Texture.",
            rectangle.x, rectangle.y, rectangle.w, rectangle.h, mipWidth, mipHeight);
    }

    if (love::IsPixelFormatCompressed(data->GetFormat()) &&
        (rectangle.x != 0 || rectangle.y != 0 || rectangle.w != mipWidth ||
         rectangle.h != mipHeight))
    {
        const auto& info = love::GetPixelFormatInfo(data->GetFormat());

        int blockWidth  = info.blockWidth;
        int blockHeight = info.blockHeight;

        if (rectangle.x % blockWidth != 0 || rectangle.y % blockHeight != 0 ||
            rectangle.w % blockWidth != 0 || rectangle.h % blockHeight != 0)
        {
            const char* name = love::GetPixelFormatName(data->GetFormat());

            throw love::Exception(
                "Compressed texture format %s only supports replacing a sub-rectangle with offset "
                "and dimensions that are a multiple of %d x %d.",
                name, blockWidth, blockHeight);
        }
    }

    this->ReplacePixels(data->GetData(), data->GetSize(), slice, mipmap, rectangle, false);
}

void Texture<Console::HAC>::ReplacePixels(const void* data, size_t size, int slice, int mipmap,
                                          const Rect& rectangle, bool reloadMipmaps)
{
    if (!data || size == 0)
        throw love::Exception("No data for replacement.");

    auto poolId       = Renderer<Console::HAC>::DATA;
    auto& scratchPool = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    auto tempImageMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempImageMemory)
        throw love::Exception("Failed to allocate temporary memory.");

    /* copy the data into the temp image memory */
    std::memcpy(tempImageMemory.getCpuAddr(), data, size);

    auto device     = Renderer<Console::HAC>::Instance().GetDevice();
    auto tempCmdBuf = dk::CmdBufMaker { device }.create();

    /* make some memory for the command buffer */
    auto tempCmdMemory = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);

    const auto& memBlock = tempCmdMemory.getMemBlock();
    const auto offset    = tempCmdMemory.getOffset();
    const auto memSize   = tempCmdMemory.getSize();

    /* add the memory to the command buffer */
    tempCmdBuf.addMemory(memBlock, offset, memSize);

    dk::ImageView view { this->image };

    DkImageRect dkRectangle {};
    dkImageRectFromRect(rectangle, dkRectangle);

    tempCmdBuf.copyBufferToImage({ tempImageMemory.getGpuAddr() }, view, dkRectangle);

    const auto queueId = Renderer<Console::HAC>::QUEUE_IMAGES;
    auto transferQueue = Renderer<Console::HAC>::Instance().GetQueue(queueId);

    transferQueue.submitCommands(tempCmdBuf.finishList());
    transferQueue.waitIdle();

    tempCmdMemory.destroy();
    tempImageMemory.destroy();
}

void Texture<Console::HAC>::Draw(Graphics<Console::HAC>& graphics, const Matrix4& matrix)
{
    this->Draw(graphics, this->quad, matrix);
}

void Texture<Console::HAC>::Draw(Graphics<Console::HAC>& graphics, Quad* quad,
                                 const Matrix4& matrix)
{
    if (!this->readable)
        throw love::Exception("Textures with non-readable formats cannot be drawn.");

    if (this->renderTarget && graphics.IsRenderTargetActive(this))
        throw love::Exception("Cannot render a Texture to itself.");

    const auto& stateTransform = graphics.GetTransform();
    bool is2D                  = stateTransform.IsAffine2DTransform();

    Matrix4 transform(stateTransform, matrix);

    DrawCommand command(4);
    command.shader  = Shader<>::STANDARD_TEXTURE;
    command.format  = vertex::CommonFormat::TEXTURE;
    command.type    = vertex::PRIMITIVE_QUADS;
    command.handles = { this };

    if (is2D)
    {
        transform.TransformXY(std::span(command.Positions().get(), command.count),
                              std::span(quad->GetVertexPositions(), command.count));
    }

    const auto* textureCoords = quad->GetVertexTextureCoords();
    command.FillVertices(graphics.GetColor(), textureCoords);

    Renderer<Console::HAC>::Instance().Render(command);
}
