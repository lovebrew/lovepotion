#include <objects/texture_ext.hpp>

#include <modules/graphics_ext.hpp>

#include <common/exception.hpp>

using namespace love;

static void createFramebufferObject(dk::Image& image, CMemPool::Handle& handle,
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

    auto poolId  = Renderer<Console::HAC>::IMAGE;
    auto scratch = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    handle = scratch.allocate(layout.getSize(), layout.getAlignment());
    image.initialize(layout, handle.getMemBlock(), handle.getOffset());

    dk::ImageView imageView { image };
    descriptor.initialize(imageView);
}

static DkImageRect dkImageRectFromRect(const Rect& rectangle)
{
    DkImageRect result {};

    result.x = (uint32_t)rectangle.x;
    result.y = (uint32_t)rectangle.y;
    result.z = (uint32_t)0;

    result.width  = (uint32_t)rectangle.w;
    result.height = (uint32_t)rectangle.h;
    result.depth  = (uint32_t)1;

    return result;
}

static void createTextureObject(dk::Image& image, CMemPool::Handle& handle,
                                dk::ImageDescriptor& descriptor, const void* data,
                                PixelFormat format, Rect rectangle)
{
    if (data == nullptr || rectangle.w * rectangle.h == 0)
        throw love::Exception("No data for Texture.");

    std::optional<DkImageFormat> imageFormat;
    if (!(imageFormat = Renderer<Console::HAC>::pixelFormats.Find(format)))
        throw love::Exception("Invalid image format.");

    const auto size = love::GetPixelFormatSliceSize(format, rectangle.w, rectangle.h);

    if (size <= 0)
        throw love::Exception("Invalid PixelFormat slice size.");

    auto poolId      = Renderer<Console::HAC>::DATA;
    auto scratchPool = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    auto tempMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempMemory)
        throw love::Exception("Failed to allocate temporary memory.");

    std::memcpy(tempMemory.getCpuAddr(), data, size);

    /*
    ** We need to have a command buffer and some more memory for it
    ** so allocate both and add the memory to the temporary command buffer
    */
    auto device            = Renderer<Console::HAC>::Instance().GetDevice();
    auto tempCommandBuffer = dk::CmdBufMaker { device }.create();
    auto tempCommandMemory = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);

    tempCommandBuffer.addMemory(tempCommandMemory.getMemBlock(), tempCommandMemory.getOffset(),
                                tempCommandMemory.getSize());

    /* set the image layout */
    dk::ImageLayout layout;
    dk::ImageLayoutMaker { device }
        .setFlags(0)
        .setFormat(*imageFormat)
        .setDimensions(rectangle.w, rectangle.h)
        .initialize(layout);

    poolId         = Renderer<Console::HAC>::IMAGE;
    auto imagePool = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    handle = imagePool.allocate(layout.getSize(), layout.getAlignment());
    image.initialize(layout, handle.getMemBlock(), handle.getOffset());
    descriptor.initialize(image);

    dk::ImageView view { image };
    DkImageRect _rectangle = dkImageRectFromRect(rectangle);

    tempCommandBuffer.copyBufferToImage({ tempMemory.getGpuAddr() }, view, _rectangle, 0);

    const auto queueId = Renderer<Console::HAC>::QUEUE_IMAGES;
    auto transferQueue = Renderer<Console::HAC>::Instance().GetQueue(queueId);

    transferQueue.submitCommands(tempCommandBuffer.finishList());
    transferQueue.waitIdle();

    tempCommandMemory.destroy();
    tempMemory.destroy();
}

Texture<Console::HAC>::Texture(const Graphics<Console::HAC>* graphics, const Settings& settings,
                               const Slices* data) :
    Texture<Console::ALL>(settings, data),
    textureHandle(0)
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
        slices = *data;

    this->LoadVolatile();

    slices.Clear();
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
    Texture<Console::ALL>::CreateTexture();
    bool hasData = this->slices.Get(0, 0) != nullptr;

    int _width  = this->pixelWidth;
    int _height = this->pixelHeight;

    Rect rectangle { 0, 0, _width, _height };

    if (this->IsRenderTarget())
    {
        bool clear = !hasData;

        auto& instance = Renderer<Console::HAC>::Instance();

        createFramebufferObject(this->image, this->handle, this->descriptor, _width, _height);

        instance.BindFramebuffer(this);
        instance.Clear({ 0, 0, 0, 0 });
        instance.BindFramebuffer(nullptr);
    }
    else
    {
        if (!hasData)
        {
            std::vector<uint8_t> empty(_width * _height, 0);
            createTextureObject(this->image, this->handle, this->descriptor, empty.data(),
                                this->format, rectangle);
        }
        else
        {
            createTextureObject(this->image, this->handle, this->descriptor,
                                this->slices.Get(0, 0)->GetData(), this->format, rectangle);
        }
    }

    Renderer<Console::HAC>::Instance().Register(this, this->textureHandle);
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
    this->handle.destroy();
}

void Texture<Console::HAC>::ReplacePixels(ImageData<Console::HAC>* data, int slice, int mipmap,
                                          int x, int y, bool reloadMipmaps)
{}

void Texture<Console::HAC>::ReplacePixels(const void* data, size_t size, int slice, int mipmap,
                                          const Rect& rect, bool reloadMipmaps)
{
    if (!data || size == 0)
        return;

    auto poolId      = Renderer<Console::HAC>::DATA;
    auto scratchPool = Renderer<Console::HAC>::Instance().GetMemPool(poolId);

    auto tempMemory = scratchPool.allocate(size, DK_IMAGE_LINEAR_STRIDE_ALIGNMENT);

    if (!tempMemory)
        throw love::Exception("Failed to allocate temporary memory.");

    std::memcpy(tempMemory.getCpuAddr(), data, size);

    /*
     ** We need to have a command buffer and some more memory for it
     ** so allocate both and add the memory to the temporary command buffer
     */
    auto device            = Renderer<Console::HAC>::Instance().GetDevice();
    auto tempCommandBuffer = dk::CmdBufMaker { device }.create();
    auto tempCommandMemory = scratchPool.allocate(DK_MEMBLOCK_ALIGNMENT);

    tempCommandBuffer.addMemory(tempCommandMemory.getMemBlock(), tempCommandMemory.getOffset(),
                                tempCommandMemory.getSize());

    dk::ImageView imageView { this->image };

    DkImageRect _rectangle = dkImageRectFromRect(rect);

    tempCommandBuffer.copyBufferToImage({ tempMemory.getGpuAddr() }, imageView, _rectangle);

    const auto queueId = Renderer<Console::HAC>::QUEUE_IMAGES;
    auto transferQueue = Renderer<Console::HAC>::Instance().GetQueue(queueId);

    transferQueue.submitCommands(tempCommandBuffer.finishList());
    transferQueue.waitIdle();

    tempCommandMemory.destroy();
    tempMemory.destroy();
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
