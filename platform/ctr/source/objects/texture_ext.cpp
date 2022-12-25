#include <objects/texture_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

using namespace love;

static void createFramebufferObject(C3D_RenderTarget*& target, C3D_Tex* texture, uint16_t width,
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

Texture<Console::CTR>::Texture(const Graphics<Console::CTR>* graphics, const Settings& settings,
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
        C3D_TexDelete(this->image.tex);

        delete this->image.tex;
    }
    else
    {
        C3D_TexDelete(this->image.tex);
        delete this->image.tex;
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
        bool clear = !hasData;
        createFramebufferObject(this->framebuffer, this->image.tex, _width, _height);
    }
    else
    {
        createTextureObject(this->image.tex, this->format, _width, _height);
        const auto copySize = love::GetPixelFormatSliceSize(this->format, _width, _height);

        if (!hasData)
            std::memset(this->image.tex->data, 0, copySize);
        else
            std::memcpy(this->image.tex->data, this->slices.Get(0, 0)->GetData(), copySize);

        C3D_TexFlush(this->image.tex);
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

void Texture<Console::CTR>::ReplacePixels(const void* data, size_t size, int slice, int mipmap,
                                          const Rect& rect, bool reloadMipmaps)
{
    unsigned sourcePowTwo      = NextPo2(rect.w);
    unsigned destinationPowTwo = NextPo2(this->width);

    for (int _y = 0; _y < std::min(rect.h, this->height - rect.y); _y++)
    {
        for (int _x = 0; _x < std::min(rect.w, this->width - rect.x); _x++)
        {
            // clang-format off
            Color color {};

            Vector2 sourcePosition { _x, _y };
            const uint32_t* sourcePixel = Color::FromTile(data, sourcePowTwo, sourcePosition);
            color                       = Color(*sourcePixel);

            Vector2 destinationPosition {(rect.x + _x), (rect.y + _y)};
            uint32_t* destinationPixel = Color::FromTile(this->image.tex, destinationPosition);
            *destinationPixel          = color.abgr();
            // clang-format on
        }
    }

    C3D_TexFlush(this->image.tex);
}

void Texture<Console::CTR>::Draw(Graphics<Console::CTR>& graphics,
                                 const Matrix4<Console::CTR>& matrix)
{
    this->Draw(graphics, this->quad, matrix);
}

[[nodiscard]] static const std::unique_ptr<Tex3DS_SubTexture> calculateSubtextureViewport(
    const Quad::Viewport& viewport, C3D_Tex* texture)
{
    std::unique_ptr<Tex3DS_SubTexture> subTexture(new Tex3DS_SubTexture());

    subTexture->top    = 1.0f - (viewport.y) / texture->height;
    subTexture->left   = viewport.x / texture->width;
    subTexture->right  = (viewport.x + viewport.w) / texture->width;
    subTexture->bottom = 1.0f - ((viewport.y + viewport.h) / texture->height);

    return subTexture;
}

void Texture<Console::CTR>::Draw(Graphics<Console::CTR>& graphics, Quad* quad,
                                 const Matrix4<Console::CTR>& matrix)
{
    if (!this->readable)
        throw love::Exception("Textures with non-readable formats cannot be drawn.");

    if (this->renderTarget && graphics.IsRenderTargetActive(this))
        throw love::Exception("Cannot render a Texture to itself.");

    const Quad::Viewport& viewport = quad->GetViewport();
    const auto uniqueSubTexture    = calculateSubtextureViewport(viewport, this->image.tex);
    this->image.subtex             = uniqueSubTexture.get();

    Matrix4<Console::CTR> transform(graphics.GetTransform(), matrix);

    C2D_DrawParams params {};
    params.pos    = { 0.0f, 0.0f, (float)viewport.w, (float)viewport.h };
    params.center = { 0.0f, 0.0f };
    params.depth  = graphics.GetCurrentDepth();
    params.angle  = 0.0f;

    C2D_ViewRestore(&transform.GetElements());

    C2D_ImageTint tint {};
    C2D_PlainImageTint(&tint, graphics.GetColor().rgba(), 1.0f);
    C2D_DrawImage(this->image, &params, &tint);
}
