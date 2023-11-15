#include <objects/texture_ext.hpp>

#include <modules/graphics_ext.hpp>
#include <utilities/driver/renderer_ext.hpp>

#include <gx2/utils.h>

#include <malloc.h>

using namespace love;

static void createFramebufferObject(GX2ColorBuffer*& buffer, GX2Texture*& texture, int width,
                                    int height)
{
    buffer = new GX2ColorBuffer();

    if (!buffer)
        throw love::Exception("Failed to create GX2Texture.");

    std::memset(&buffer->surface, 0, sizeof(GX2Surface));

    buffer->surface.use       = GX2_SURFACE_USE_TEXTURE_COLOR_BUFFER_TV;
    buffer->surface.dim       = GX2_SURFACE_DIM_TEXTURE_2D;
    buffer->surface.aa        = GX2_AA_MODE1X;
    buffer->surface.width     = width;
    buffer->surface.height    = height;
    buffer->surface.depth     = 1;
    buffer->surface.mipLevels = 1;
    buffer->surface.format    = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
    buffer->surface.swizzle   = 0;
    buffer->surface.tileMode  = GX2_TILE_MODE_LINEAR_ALIGNED;
    buffer->surface.mipmaps   = nullptr;
    buffer->viewFirstSlice    = 0;
    buffer->viewMip           = 0;
    buffer->viewNumSlices     = 1;

    GX2CalcSurfaceSizeAndAlignment(&buffer->surface);
    GX2InitColorBufferRegs(buffer);

    const auto size      = buffer->surface.imageSize;
    const auto alignment = buffer->surface.alignment;

    auto handle           = MEMGetBaseHeapHandle(MEM_BASE_HEAP_MEM1);
    buffer->surface.image = MEMAllocFromFrmHeapEx(handle, size, alignment);

    texture->surface.image = buffer->surface.image;
}

static void createTextureObject(GX2Texture*& texture, PixelFormat format, int width, int height)
{
    texture = new GX2Texture();

    if (!texture)
        throw love::Exception("Failed to create GX2Texture.");

    std::memset(&texture->surface, 0, sizeof(GX2Surface));

    texture->surface.use    = GX2_SURFACE_USE_TEXTURE;
    texture->surface.dim    = GX2_SURFACE_DIM_TEXTURE_2D;
    texture->surface.width  = width;
    texture->surface.height = height;

    texture->surface.depth     = 1;
    texture->surface.mipLevels = 1;

    std::optional<GX2SurfaceFormat> gxFormat;
    if (!(gxFormat = Renderer<Console::CAFE>::pixelFormats.Find(format)))
        throw love::Exception("Invalid pixel format.");

    texture->surface.format   = *gxFormat;
    texture->surface.aa       = GX2_AA_MODE1X;
    texture->surface.tileMode = GX2_TILE_MODE_LINEAR_ALIGNED;
    texture->viewFirstMip     = 0;
    texture->viewNumMips      = 1;
    texture->viewFirstSlice   = 0;
    texture->viewNumSlices    = 1;
    texture->compMap = GX2_COMP_MAP(GX2_SQ_SEL_R, GX2_SQ_SEL_G, GX2_SQ_SEL_B, GX2_SQ_SEL_A);

    GX2CalcSurfaceSizeAndAlignment(&texture->surface);
    GX2InitTextureRegs(texture);

    texture->surface.image = memalign(texture->surface.alignment, texture->surface.imageSize);

    if (!texture->surface.image)
        throw love::Exception("Failed to create GX2Surface.");

    std::memset(texture->surface.image, 0, texture->surface.imageSize);

    GX2Invalidate(GX2_INVALIDATE_MODE_CPU_TEXTURE, texture->surface.image,
                  texture->surface.imageSize);
}

Texture<Console::CAFE>::Texture(const Graphics<Console::CAFE>* graphics, const Settings& settings,
                                const Slices* data) :
    Texture<Console::ALL>(settings, data),
    framebuffer(nullptr),
    texture(nullptr),
    sampler {}
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

Texture<Console::CAFE>::~Texture()
{
    this->UnloadVolatile();
}

bool Texture<Console::CAFE>::LoadVolatile()
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

void Texture<Console::CAFE>::CreateTexture()
{
    Texture<Console::ALL>::CreateTexture();
    bool hasData = this->slices.Get(0, 0) != nullptr;

    int _width  = this->pixelWidth;
    int _height = this->pixelHeight;

    if (this->IsRenderTarget())
    {
        bool clear = !hasData;

        createTextureObject(this->texture, PixelFormat::PIXELFORMAT_RGBA8_UNORM, width, height);
        createFramebufferObject(this->framebuffer, this->texture, _width, _height);

        if (clear)
        {
            Renderer<Console::CAFE>::Instance().BindFramebuffer(this);
            Renderer<Console::CAFE>::Instance().Clear({ 0, 0, 0, 0 });
            Renderer<Console::CAFE>::Instance().BindFramebuffer();
        }
    }
    else
    {
        createTextureObject(this->texture, this->format, _width, _height);

        if (!hasData)
        {
            std::vector<uint8_t> empty(_width * _height, 0);
            this->ReplacePixels(empty.data(), empty.size(), 0, 0, { 0, 0, _width, _height }, false);
        }
        else
            this->ReplacePixels(this->slices.Get(0, 0), 0, 0, 0, 0, false);
    }

    this->SetSamplerState(this->state);
}

void Texture<Console::CAFE>::UnloadVolatile()
{
    if (this->texture)
        delete this->texture;

    if (this->framebuffer)
        delete this->framebuffer;
}

void Texture<Console::CAFE>::ReplacePixels(ImageDataBase* data, int slice, int mipmap, int x, int y,
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

    if (this->texture == nullptr)
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

    this->ReplacePixels(data->GetData(), data->GetSize(), 0, 0, rectangle, false);
}

void Texture<Console::CAFE>::ReplacePixels(const void* data, size_t size, int slice, int mipmap,
                                           const Rect& rectangle, bool reloadMipmaps)
{
    if (!this->IsReadable() || this->GetMSAA() > 1)
        return;

    auto* graphics = Module::GetInstance<Graphics<Console::CAFE>>(Module::M_GRAPHICS);

    if (graphics != nullptr && graphics->IsRenderTargetActive(this))
        return;

    const auto pitch = this->texture->surface.pitch;

    uint8_t* dest   = (uint8_t*)this->texture->surface.image;
    uint8_t* source = (uint8_t*)data;

    size_t pixelSize = GetPixelFormatBlockSize(this->format);

    /* copy by row */
    for (uint32_t y = 0; y < (uint32_t)rectangle.h; y++)
    {
        const auto srcRow  = (y * rectangle.w * pixelSize);
        const auto destRow = (rectangle.x + (y + rectangle.y) * pitch) * pixelSize;

        std::memcpy(dest + destRow, source + srcRow, rectangle.w * pixelSize);
    }

    const auto imageSize = this->texture->surface.imageSize;
    GX2Invalidate(Texture::INVALIDATE_MODE, this->texture->surface.image, imageSize);
}

void Texture<Console::CAFE>::Draw(Graphics<Console::CAFE>& graphics, const Matrix4& matrix)
{
    this->Draw(graphics, this->quad, matrix);
}

void Texture<Console::CAFE>::Draw(Graphics<Console::CAFE>& graphics, Quad* quad,
                                  const Matrix4& matrix)
{
    if (!this->readable)
        throw love::Exception("Textures with non-readable formats cannot be drawn.");

    if (this->renderTarget && graphics.IsRenderTargetActive(this))
        throw love::Exception("Cannot render a Texture to itself.");

    const auto& stateTransform = graphics.GetTransform();
    bool is2D                  = stateTransform.IsAffine2DTransform();

    Matrix4 transform(stateTransform, matrix);

    love::DrawCommand command(4);
    command.shader  = Shader<>::STANDARD_TEXTURE;
    command.format  = vertex::CommonFormat::TEXTURE;
    command.type    = vertex::PRIMITIVE_QUADS;
    command.handles = { this };

    if (is2D)
        transform.TransformXY(std::span(command.Positions().get(), command.count),
                              std::span(quad->GetVertexPositions(), command.count));

    const auto* textureCoords = quad->GetVertexTextureCoords();
    command.FillVertices(graphics.GetColor(), textureCoords);

    Renderer<Console::CAFE>::Instance().Render(command);
}

void Texture<Console::CAFE>::SetSamplerState(const SamplerState& state)
{
    Texture<>::SetSamplerState(state);

    this->state.magFilter = this->state.minFilter = SamplerState::FILTER_NEAREST;

    if (this->state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        this->state.mipmapFilter = SamplerState::MIPMAP_FILTER_NEAREST;

    Renderer<Console::CAFE>::Instance().SetSamplerState(this, this->state);
}
