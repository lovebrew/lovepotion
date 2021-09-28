#include <3ds.h>
#include <citro2d.h>

#include "common/colors.h"
#include "common/luax.h"

#include "citro2d/citro.h"

#include "modules/graphics/graphics.h"

citro2d::citro2d()
{
    gfxInitDefault();
    gfxSet3D(true);

    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
        svcBreak(USERBREAK_PANIC);

    if (!C2D_Init(C2D_DEFAULT_MAX_OBJECTS))
        svcBreak(USERBREAK_PANIC);

    C2D_Prepare();

    C2D_Flush();
    C3D_AlphaTest(true, GPU_GREATER, 0);

    C2D_SetTintMode(C2D_TintMult);

    this->targets.reserve(4);

    this->targets = { C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT),
                      C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT),
                      C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT) };

    love::Texture::Filter filter;
    filter.min = filter.mag = love::Texture::FILTER_NEAREST;
    this->SetTextureFilter(filter);

    love::Texture::Wrap wrap;
    wrap.s = wrap.t = wrap.r = love::Texture::WRAP_CLAMP;
    this->SetTextureWrap(wrap);
}

citro2d::~citro2d()
{
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

citro2d& citro2d::Instance()
{
    static citro2d c2d;
    return c2d;
}

void citro2d::SetBlendMode(GPU_BLENDEQUATION func, GPU_BLENDFACTOR srcColor,
                           GPU_BLENDFACTOR srcAlpha, GPU_BLENDFACTOR dstColor,
                           GPU_BLENDFACTOR dstAlpha)
{
    C2D_Flush();
    C3D_AlphaBlend(func, func, srcColor, dstColor, srcAlpha, dstAlpha);
}

void citro2d::SetColorMask(const love::Graphics::ColorMask& mask)
{
    C2D_Flush();

    uint8_t writeMask = GPU_WRITE_DEPTH;
    writeMask |= mask.GetColorMask();

    C3D_DepthTest(true, GPU_GEQUAL, static_cast<GPU_WRITEMASK>(writeMask));
}

void citro2d::Set3D(bool enable)
{
    gfxSet3D(enable);
}

bool citro2d::Get3D() const
{
    return gfxIs3D();
}

void citro2d::EnsureInFrame()
{
    if (!this->inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        this->inFrame = true;
    }
}

void citro2d::BindFramebuffer(love::Canvas* canvas)
{
    this->EnsureInFrame();

    if (canvas != nullptr)
        this->current = canvas->GetRenderer();
    else
        this->current = this->targets[love::Graphics::ACTIVE_SCREEN];

    C2D_SceneBegin(this->current);
}

void citro2d::ClearColor(const Colorf& color)
{
    C2D_TargetClear(this->current, C2D_Color32f(color.r, color.g, color.b, color.a));
}

void citro2d::Present()
{
    if (this->inFrame)
    {
        C3D_FrameEnd(0);
        this->inFrame = false;
    }

    for (size_t i = this->deferredFunctions.size(); i > 0; i--)
    {
        this->deferredFunctions[i - 1]();
        this->deferredFunctions.erase(deferredFunctions.begin() + i - 1);
    }
}

void citro2d::SetScissor(GPU_SCISSORMODE mode, const love::Rect& scissor, int screenWidth,
                         bool canvasActive)
{
    C3D_SetScissor(mode, 240 - (scissor.y + scissor.h), screenWidth - (scissor.x + scissor.w),
                   240 - scissor.y, screenWidth - scissor.x);
}

void citro2d::SetStencil(GPU_TESTFUNC compare, int value)
{
    bool enabled = (compare == GPU_ALWAYS) ? false : true;

    C3D_StencilTest(enabled, compare, value, 0xFFFFFFFF, 0xFFFFFFFF);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
}

// Set the global filter mode for textures
void citro2d::SetTextureFilter(const love::Texture::Filter& filter)
{
    GPU_TEXTURE_FILTER_PARAM min =
        (filter.min == love::Texture::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;
    GPU_TEXTURE_FILTER_PARAM mag =
        (filter.min == love::Texture::FILTER_NEAREST) ? GPU_NEAREST : GPU_LINEAR;

    GPU_PROCTEX_FILTER mipFilter = GPU_PT_LINEAR;

    if (filter.mipmap != love::Texture::FILTER_NONE)
    {
        if (filter.min == love::Texture::FILTER_NEAREST &&
            filter.mipmap == love::Texture::FILTER_NEAREST)
            mipFilter = GPU_PT_NEAREST;
        else if (filter.min == love::Texture::FILTER_NEAREST &&
                 filter.mipmap == love::Texture::FILTER_LINEAR)
            mipFilter = GPU_PT_LINEAR;
        else if (filter.min == love::Texture::FILTER_LINEAR &&
                 filter.mipmap == love::Texture::FILTER_NEAREST)
            mipFilter = GPU_PT_NEAREST;
        else if (filter.min == love::Texture::FILTER_LINEAR &&
                 filter.mipmap == love::Texture::FILTER_LINEAR)
            mipFilter = GPU_PT_LINEAR;
        else
            mipFilter = GPU_PT_LINEAR;
    }

    this->filter.min = min;
    this->filter.mag = mag;

    this->filter.mipMap = mipFilter;
}

void citro2d::SetTextureFilter(love::Texture* texture, const love::Texture::Filter& filter)
{
    this->SetTextureFilter(filter);

    const C2D_Image& image = texture->GetHandle();
    C3D_TexSetFilter(image.tex, this->filter.mag, this->filter.min);
}

void citro2d::SetTextureWrap(const love::Texture::Wrap& wrap)
{
    GPU_TEXTURE_WRAP_PARAM u = citro2d::GetCitroWrapMode(wrap.s);
    GPU_TEXTURE_WRAP_PARAM v = citro2d::GetCitroWrapMode(wrap.t);

    this->wrap.s = u;
    this->wrap.t = v;
}

void citro2d::SetTextureWrap(love::Texture* texture, const love::Texture::Wrap& wrap)
{
    this->SetTextureWrap(wrap);

    const C2D_Image& image = texture->GetHandle();
    C3D_TexSetWrap(image.tex, this->wrap.s, this->wrap.t);
}

GPU_TEXTURE_FILTER_PARAM citro2d::GetCitroFilterMode(love::Texture::FilterMode mode)
{
    switch (mode)
    {
        default:
        case love::Texture::FilterMode::FILTER_LINEAR:
            return GPU_LINEAR;
        case love::Texture::FilterMode::FILTER_NEAREST:
            return GPU_NEAREST;
    }
}

citro2d::GPUFilter citro2d::GetCitroFilterMode(const love::Texture::Filter& filter)
{
    citro2d::GPUFilter gpuFilter {};

    gpuFilter.mag = citro2d::GetCitroFilterMode(filter.mag);
    gpuFilter.min = citro2d::GetCitroFilterMode(filter.min);

    return gpuFilter;
}

GPU_TEXTURE_WRAP_PARAM citro2d::GetCitroWrapMode(love::Texture::WrapMode wrap)
{
    switch (wrap)
    {
        case love::Texture::WRAP_CLAMP:
        default:
            return GPU_CLAMP_TO_EDGE;
        case love::Texture::WRAP_CLAMP_ZERO:
            return GPU_CLAMP_TO_BORDER;
        case love::Texture::WRAP_REPEAT:
            return GPU_REPEAT;
        case love::Texture::WRAP_MIRRORED_REPEAT:
            return GPU_MIRRORED_REPEAT;
    }
}
