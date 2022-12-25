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
                                dk::ImageDescriptor& descriptor, const void* data,
                                PixelFormat format, Rect rectangle)
{
    if (data == nullptr)
        throw love::Exception("No data for Texture.");

    std::optional<DkImageFormat> imageFormat;
    if (!(imageFormat = Renderer<Console::HAC>::pixelFormats.Find(format)))
        throw love::Exception("Invalid image format.");

    const auto size = love::GetPixelFormatSliceSize(format, rectangle.w, rectangle.h);

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

        auto& instance = Renderer<Console::HAC>::Instance();

        createFramebufferObject(this->image, this->memory, this->descriptor, _width, _height);

        instance.BindFramebuffer(this);
        instance.Clear({ 0, 0, 0, 0 });
        instance.BindFramebuffer(nullptr);
    }
    else
    {
        if (!hasData)
        {
            std::vector<uint8_t> empty(_width * _height, 0);
            createTextureObject(this->image, this->memory, this->descriptor, empty.data(),
                                this->format, rectangle);
        }
        else
        {
            createTextureObject(this->image, this->memory, this->descriptor,
                                this->slices.Get(0, 0)->GetData(), this->format, rectangle);
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

void Texture<Console::HAC>::ReplacePixels(ImageData<Console::HAC>* data, int slice, int mipmap,
                                          int x, int y, bool reloadMipmaps)
{
    this->ReplacePixels(data->GetData(), data->GetSize(), slice, mipmap,
                        { x, y, data->GetWidth(), data->GetHeight() }, false);
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

void Texture<Console::HAC>::Draw(Graphics<Console::HAC>& graphics,
                                 const Matrix4<Console::HAC>& matrix)
{
    this->Draw(graphics, this->quad, matrix);
}

void Texture<Console::HAC>::Draw(Graphics<Console::HAC>& graphics, Quad* quad,
                                 const Matrix4<Console::HAC>& matrix)
{
    if (!this->readable)
        throw love::Exception("Textures with non-readable formats cannot be drawn.");

    if (this->renderTarget && graphics.IsRenderTargetActive(this))
        throw love::Exception("Cannot render a Texture to itself.");

    const auto& stateTransform = graphics.GetTransform();
    bool is2D                  = stateTransform.IsAffine2DTransform();

    Matrix4<Console::HAC> transform(stateTransform, matrix);

    auto command         = Graphics<Console::HAC>::DrawCommand(4);
    command.shader       = Shader<>::STANDARD_TEXTURE;
    command.format       = vertex::CommonFormat::TEXTURE;
    command.primitveType = vertex::PRIMITIVE_QUADS;
    command.handles      = { this->textureHandle };

    if (is2D)
        transform.TransformXY(command.Positions().get(), quad->GetVertexPositions(), command.count);

    const auto* textureCoords = quad->GetVertexTextureCoords();
    command.FillVertices(graphics.GetColor(), textureCoords);

    Renderer<Console::HAC>::Instance().Render(command);
}