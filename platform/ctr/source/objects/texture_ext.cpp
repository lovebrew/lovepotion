#include <objects/texture_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

using namespace love;

static void createFramebufferObject(C3D_RenderTarget*& target, C3D_Tex*& texture, uint16_t width,
                                    uint16_t height)
{
    const auto _width  = NextPo2(width);
    const auto _height = NextPo2(height);

    texture = new C3D_Tex();

    if (!C3D_TexInitVRAM(texture, _width, _height, GPU_RGBA8))
        throw love::Exception("Failed to create framebuffer Texture");

    target = C3D_RenderTargetCreateFromTex(texture, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH16);
}

static void createTextureObject(C3D_Tex*& texture, PixelFormat format, uint16_t width,
                                uint16_t height)
{
    const auto _width  = NextPo2(width);
    const auto _height = NextPo2(height);

    texture = new C3D_Tex();

    std::optional<GPU_TEXCOLOR> color;
    if (!(color = Renderer<Console::CTR>::pixelFormats.Find(format)))
        throw love::Exception("Invalid color format: %s", love::GetPixelFormatName(format));

    if (!C3D_TexInit(texture, _width, _height, *color))
        throw love::Exception("Failed to create Texture!");
}

Texture<Console::CTR>::Texture(const Graphics<Console::ALL>* graphics, const Settings& settings,
                               const Slices* data) :
    Texture<Console::ALL>(settings, data),
    framebuffer(nullptr)
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

    this->state = graphics->GetDefaultSamplerState();

    Quad::Viewport view { 0, 0, (double)this->width, (double)this->height };
    this->quad.Set(new Quad(view, this->width, this->height), Acquire::NORETAIN);

    ++textureCount;

    if (data != nullptr)
        slices = *data;

    this->LoadVolatile();

    slices.Clear();
}

Texture<Console::CTR>::~Texture()
{
    this->UnloadVolatile();
}

bool Texture<Console::CTR>::LoadVolatile()
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

void Texture<Console::CTR>::UnloadVolatile()
{
    if (this->IsRenderTarget() && this->framebuffer != nullptr)
    {
        C3D_RenderTargetDelete(this->framebuffer);
        C3D_TexDelete(this->texture);

        delete this->texture;
    }
    else
    {
        C3D_TexDelete(this->texture);
        delete this->texture;
    }
}

void Texture<Console::CTR>::SetSamplerState(const SamplerState& state)
{
    Texture<>::SetSamplerState(state);

    this->state.magFilter = this->state.minFilter = SamplerState::FILTER_NEAREST;

    if (this->state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        this->state.mipmapFilter = SamplerState::MIPMAP_FILTER_NEAREST;

    Renderer<Console::CTR>::Instance().SetSamplerState(this, this->state);
}

void Texture<Console::CTR>::CreateTexture()
{
    Texture<Console::ALL>::CreateTexture();
    bool hasData = this->slices.Get(0, 0) != nullptr;

    int _width  = this->pixelWidth;
    int _height = this->pixelHeight;

    if (this->IsRenderTarget())
    {
        createFramebufferObject(this->framebuffer, this->texture, _width, _height);

        if (!hasData)
        {
            Renderer<Console::CTR>::Instance().BindFramebuffer(this);
            Renderer<Console::CTR>::Instance().Clear({ 0, 0, 0, 0 });
            Renderer<Console::CTR>::Instance().BindFramebuffer();
        }
    }
    else
    {
        createTextureObject(this->texture, this->format, _width, _height);
        const auto copySize = love::GetPixelFormatSliceSize(this->format, _width, _height);

        if (!hasData)
            std::memset(this->texture->data, 0, copySize);
        else
            std::memcpy(this->texture->data, this->slices.Get(0, 0)->GetData(), copySize);

        C3D_TexFlush(this->texture);
    }

    this->SetSamplerState(this->state);
}

void Texture<Console::CTR>::ReplacePixels(ImageData<Console::CTR>* data, int slice, int mipmap,
                                          int x, int y, bool reloadMipmaps)
{
    if (!this->IsReadable())
        throw love::Exception("replacePixels can only be called on a readable Texture.");

    if (this->GetMSAA() > 1)
        throw love::Exception("replacePixels cannot be called on a MSAA Texture.");

    auto* graphics = Module::GetInstance<Graphics<Console::CTR>>(Module::M_GRAPHICS);
    if (graphics == nullptr && graphics->IsRenderTargetActive(this))
        throw love::Exception(
            "replacePixels cannot be called on this Texture while it's an active rendertarget.");

    if (this->GetHandle() == nullptr)
        return;

    if (data->GetFormat() != this->GetPixelFormat())
        throw love::Exception("Pixel formats must match.");

    if (mipmap < 0 || mipmap >= this->GetMipmapCount())
        throw love::Exception("Invalid Texture mipmap index: %d", mipmap + 1);

    // clang-format off
    bool isInvalidCubeslice   = this->textureType == TEXTURE_CUBE && slice >= 6;
    bool isInvalidVolumeSlice = this->textureType == TEXTURE_VOLUME && slice >= this->GetDepth(mipmap);
    bool isInvalidArraySlice  = this->textureType == TEXTURE_2D_ARRAY && slice >= this->GetLayerCount();
    // clang-format on

    if (slice < 0 || isInvalidCubeslice || isInvalidVolumeSlice || isInvalidArraySlice)
        throw love::Exception("Invalid texture slice index %d.", slice + 1);

    Rect rect = { x, y, data->GetWidth(), data->GetHeight() };

    int mipWidth  = this->GetPixelWidth(mipmap);
    int mipHeight = this->GetPixelHeight(mipmap);

    if (rect.x < 0 || rect.y < 0 || rect.w <= 0 || rect.h <= 0 || (rect.x + rect.w) > mipWidth ||
        (rect.y + rect.h) > mipHeight)
    {
        throw love::Exception("Invalid rectangle dimensions (x = %d, y = %d, w = %d, h = %d) "
                              "for a %dx%d Texture.",
                              rect.x, rect.y, rect.w, rect.h, mipWidth, mipHeight);
    }

    this->ReplacePixels(data->GetData(), data->GetSize(), slice, mipmap, rect, reloadMipmaps);
}

template<typename T>
void _replacePixels(const void* source, void* texture, const Rect& rect, const int width,
                    const int height)
{
    const auto sourcePowTwo = NextPo2(rect.w);
    const auto destPowTwo   = NextPo2(width);

    for (int _y = 0; _y < std::min(rect.h, height - rect.y); _y++)
    {
        for (int _x = 0; _x < std::min(rect.w, width - rect.x); _x++)
        {
            Vector2 srcPosition { _x, _y };
            const auto* srcPixel = Color::FromTile<T>(source, sourcePowTwo, srcPosition);

            Vector2 destPosition { (rect.x + _x), (rect.y + _y) };
            auto* destPixel = Color::FromTile<T>(texture, destPowTwo, destPosition);
            *destPixel      = *srcPixel;
        }
    }
}

void Texture<Console::CTR>::ReplacePixels(const void* data, size_t size, int slice, int mipmap,
                                          const Rect& rect, bool reloadMipmaps)
{
    switch (this->GetPixelFormat())
    {
        case PIXELFORMAT_RGB565_UNORM:
            _replacePixels<uint16_t>(data, this->texture->data, rect, this->width, this->height);
            break;
        default:
            _replacePixels<uint32_t>(data, this->texture->data, rect, this->width, this->height);
            break;
    }

    C3D_TexFlush(this->texture);
}

void Texture<Console::CTR>::Draw(Graphics<Console::CTR>& graphics,
                                 const Matrix4<Console::CTR>& matrix)
{
    this->Draw(graphics, this->quad, matrix);
}

static Vector2 getVertex(const float x, const float y, double width, double height)
{
    const auto u = (double)y / width;
    const auto v = (width - x) / height;

    return Vector2(u, v);
}

static void setQuad(StrongReference<Quad> quad, const Quad::Viewport& viewport, const double width,
                    const double height, bool isRendertarget)
{
    if (quad == nullptr)
        throw love::Exception("Invalid quad (is nullptr).");

    if (isRendertarget)
    {
        quad->SetVertexTextureCoord(0, getVertex(0.0f, 0.0f, width, height));
        quad->SetVertexTextureCoord(1, getVertex(0.0f, height, width, height));
        quad->SetVertexTextureCoord(2, getVertex(width, height, width, height));
        quad->SetVertexTextureCoord(3, getVertex(width, 0.0f, width, height));

        return;
    }

    quad.Set(new Quad(viewport, width, height), Acquire::NORETAIN);
}

void Texture<Console::CTR>::Draw(Graphics<Console::CTR>& graphics, Quad* quad,
                                 const Matrix4<Console::CTR>& matrix)
{
    if (!this->readable)
        throw love::Exception("Textures with non-readable formats cannot be drawn.");

    if (this->renderTarget && graphics.IsRenderTargetActive(this))
        throw love::Exception("Cannot render a Texture to itself.");

    const Quad::Viewport& viewport = quad->GetViewport();
    setQuad(this->quad, viewport, this->texture->width, this->texture->height, this->renderTarget);

    const auto& transform = graphics.GetTransform();
    bool is2D             = transform.IsAffine2DTransform();

    Matrix4<Console::CTR> translated(graphics.GetTransform(), matrix);

    DrawCommand<Console::CTR> command(0x04, vertex::PRIMITIVE_TRIANGLE_FAN);
    command.handles = { this->texture };
    command.format  = CommonFormat::TEXTURE;

    if (is2D)
        translated.TransformXY(command.Positions().get(), this->quad->GetVertexPositions(),
                               command.count);

    const auto* coords = this->quad->GetVertexTextureCoords();
    command.FillVertices(graphics.GetColor(), coords);

    Renderer<Console::CTR>::Instance().Render(command);
}
