#include <objects/texture_ext.hpp>

#include <utilities/driver/renderer_ext.hpp>

using namespace love;

static void createFramebufferObject(C3D_RenderTarget*& target, C3D_Tex* texture, uint16_t width,
                                    uint16_t height)
{
    const auto _width  = NextPo2(width);
    const auto _height = NextPo2(height);

    if (!C3D_TexInitVRAM(texture, _width, _height, GPU_RGBA8))
        throw love::Exception("Failed to create framebuffer Texture");

    target = C3D_RenderTargetCreateFromTex(texture, GPU_TEXFACE_2D, 0, GPU_RB_DEPTH16);
}

static void createTextureObject(C3D_Tex*& texture, PixelFormat format, uint16_t width,
                                uint16_t height)
{
    const auto _width  = NextPo2(width);
    const auto _height = NextPo2(height);

    std::optional<GPU_TEXCOLOR> color;
    if (!(color = Renderer<Console::CTR>::pixelFormats.Find(format)))
        throw love::Exception("Invalid color format: %s", love::GetPixelFormatName(format));

    if (!C3D_TexInit(texture, _width, _height, *color))
        throw love::Exception("Failed to create Texture!");
}

Texture<Console::CTR>::Texture(Graphics<Console::CTR>* graphics, const Settings& settings,
                               const Slices* data) :
    Texture<Console::ALL>(graphics, settings, data),
    framebuffer(nullptr)
{
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
    }
    else
    {
        C3D_TexDelete(this->image.tex);
        delete this->image.tex;
    }
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
}

void Texture<Console::CTR>::Draw(Graphics<Console::CTR>& graphics,
                                 const Matrix4<Console::CTR>& matrix)
{
    this->Draw(graphics, this->quad, matrix);
}

void Texture<Console::CTR>::Draw(Graphics<Console::CTR>& graphics, Quad* quad,
                                 const Matrix4<Console::CTR>& matrix)
{
    if (!this->readable)
        throw love::Exception("Textures with non-readable formats cannot be drawn.");

    // if (this->renderTarget && graphics->IsRenderTargetActive(this))
    //     throw love::Exception("Cannot render a Texture to itself.");

    const Quad::Viewport& viewport = quad->GetViewport();

    Matrix4 transform(graphics->GetTransform(), matrix);

    C2D_DrawParams params {
        .pos    = { 0.0f, 0.0f, (float)viewport.w, (float)viewport.h },
        .depth  = Graphics::CURRENT_DEPTH,
        .angle  = 0.0f,
        .center = { 0.0f, 0.0f }
    };

    C2D_ViewRestore(&transform.GetElements());

    C2D_ImageTint tint {};
    C2D_PlainImageTint(&tint, graphics->GetColor().rgba());
    C2D_DrawImage(this->image, &params, &tint);
}
