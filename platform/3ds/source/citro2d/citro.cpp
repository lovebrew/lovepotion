#include <3ds.h>

#include "citro2d/citro.h"

#include "common/luax.h"
#include "common/pixelformat.h"

#include "modules/graphics/graphics.h"

#include "objects/canvas/canvas.h"
#include "objects/texture/texture.h"

using namespace love;

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

    this->InitRendererInfo();

    this->targets.fill(nullptr);
    this->CreateFramebuffers();
}

citro2d::~citro2d()
{
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

void citro2d::InitRendererInfo()
{
    memset(&this->rendererInfo, 0, sizeof(RendererInfo));

    this->rendererInfo.device  = RENDERER_DEVICE;
    this->rendererInfo.name    = RENDERER_NAME;
    this->rendererInfo.vendor  = RENDERER_VENDOR;
    this->rendererInfo.version = RENDERER_VERSION;
}

void citro2d::DestroyFramebuffers()
{
    for (auto framebuffer : this->targets)
        C3D_RenderTargetDelete(framebuffer);
}

void citro2d::CreateFramebuffers()
{
    for (size_t index = 0; index < citro2d::MAX_RENDERTARGETS; index++)
    {
        gfxScreen_t screen = (index < 2) ? GFX_TOP : GFX_BOTTOM;
        gfx3dSide_t side   = (index != 1) ? GFX_LEFT : GFX_RIGHT;

        this->targets[index] = C2D_CreateScreenTarget(screen, side);
    }
}

void citro2d::Clear(const Colorf& color)
{
    C2D_TargetClear(this->current, C2D_Color32f(color.r, color.g, color.b, color.a));
}

void citro2d::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{}

void citro2d::SetBlendColor(const Colorf& color)
{}

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

void citro2d::SetViewport(const Rect& viewport)
{
    C2D_Flush();
    C3D_SetViewport(viewport.x, viewport.y, viewport.w, viewport.h);

    this->viewport = viewport;
}

Rect citro2d::GetViewport() const
{
    return this->viewport;
}

void citro2d::SetScissor(bool enable, const Rect& scissor, bool canvasActive)
{
    GPU_SCISSORMODE mode = enable ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

    if (enable)
        C2D_Flush();

    size_t width = Screen::Instance().GetWidth(Graphics::ACTIVE_SCREEN);

    uint32_t left   = 240 > (scissor.y + scissor.h) ? 240 - (scissor.y + scissor.h) : 0;
    uint32_t top    = width > (scissor.x + scissor.w) ? width - (scissor.x + scissor.w) : 0;
    uint32_t right  = 240 - scissor.y;
    uint32_t bottom = width - scissor.x;

    C3D_SetScissor(mode, left, top, right, bottom);
}

void citro2d::SetStencil(RenderState::CompareMode compare, int value)
{
    bool enabled = (compare == RenderState::COMPARE_ALWAYS) ? false : true;

    GPU_TESTFUNC compareOp;
    ::citro2d::GetConstant(compare, compareOp);

    C3D_StencilTest(enabled, compareOp, value, 0xFFFFFFFF, 0xFFFFFFFF);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
}

void citro2d::SetMeshCullMode(Vertex::CullMode mode)
{
    GPU_CULLMODE cullMode;
    ::citro2d::GetConstant(mode, cullMode);

    C2D_Flush();
    C3D_CullFace(cullMode);
}

void citro2d::SetVertexWinding(Vertex::Winding winding)
{}

void citro2d::SetSamplerState(Texture* texture, SamplerState& state)
{
    GPU_TEXTURE_FILTER_PARAM min;
    ::citro2d::GetConstant(state.minFilter, min);

    GPU_TEXTURE_FILTER_PARAM mag;
    ::citro2d::GetConstant(state.magFilter, mag);

    GPU_TEXTURE_FILTER_PARAM mipFilter;

    if (state.mipmapFilter != SamplerState::MIPMAP_FILTER_NONE)
    {
        if (state.minFilter == SamplerState::FILTER_NEAREST &&
            state.mipmapFilter == SamplerState::MIPMAP_FILTER_NEAREST)
        {
            mipFilter = GPU_NEAREST;
        }
        else if (state.minFilter == SamplerState::FILTER_NEAREST &&
                 state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        {
            mipFilter = GPU_LINEAR;
        }
        else if (state.magFilter == SamplerState::FILTER_LINEAR &&
                 state.mipmapFilter == SamplerState::MIPMAP_FILTER_NEAREST)
        {
            mipFilter = GPU_NEAREST;
        }
        else if (state.magFilter == SamplerState::FILTER_LINEAR &&
                 state.mipmapFilter == SamplerState::MIPMAP_FILTER_LINEAR)
        {
            mipFilter = GPU_LINEAR;
        }
    }

    C3D_TexSetFilter(texture->GetHandle().tex, mag, min);
    C3D_TexSetFilterMipmap(texture->GetHandle().tex, mipFilter);

    if (SamplerState::IsClampZeroOrOne(state.wrapU))
        state.wrapU = SamplerState::WRAP_CLAMP;

    if (SamplerState::IsClampZeroOrOne(state.wrapV))
        state.wrapV = SamplerState::WRAP_CLAMP;

    GPU_TEXTURE_WRAP_PARAM wrapU;
    citro2d::GetConstant(state.wrapU, wrapU);

    GPU_TEXTURE_WRAP_PARAM wrapV;
    citro2d::GetConstant(state.wrapV, wrapV);

    C3D_TexSetWrap(texture->GetHandle().tex, wrapU, wrapV);
}

void citro2d::SetColorMask(const RenderState::ColorMask& mask)
{
    C2D_Flush();

    uint8_t writeMask = GPU_WRITE_DEPTH;
    writeMask |= mask.GetColorMask();

    C3D_DepthTest(true, GPU_GEQUAL, static_cast<GPU_WRITEMASK>(writeMask));
}

void citro2d::SetBlendMode(const RenderState::BlendState& blend)
{
    GPU_BLENDEQUATION opRGB;
    citro2d::GetConstant(blend.operationRGB, opRGB);

    GPU_BLENDEQUATION opAlpha;
    citro2d::GetConstant(blend.operationA, opAlpha);

    GPU_BLENDFACTOR srcColor;
    citro2d::GetConstant(blend.srcFactorRGB, srcColor);

    GPU_BLENDFACTOR dstColor;
    citro2d::GetConstant(blend.dstFactorRGB, dstColor);

    GPU_BLENDFACTOR srcAlpha;
    citro2d::GetConstant(blend.srcFactorA, srcAlpha);

    GPU_BLENDFACTOR dstAlpha;
    citro2d::GetConstant(blend.dstFactorA, dstAlpha);

    C2D_Flush();
    C3D_AlphaBlend(opRGB, opAlpha, srcColor, dstColor, srcAlpha, dstAlpha);
}

void citro2d::SetLineWidth(float lineWidth)
{}

void citro2d::SetLineStyle(Graphics::LineStyle style)
{}

void citro2d::SetPointSize(float pointSize)
{}

// clang-format off
constexpr auto pixelFormats = BidirectionalMap<>::Create(
    PIXELFORMAT_RGBA8_UNORM,  GPU_RGBA8,
    PIXELFORMAT_RGB8,         GPU_RGB8,
    PIXELFORMAT_RGB565_UNORM, GPU_RGB565,
    PIXELFORMAT_LA8_UNORM,    GPU_LA8,
    PIXELFORMAT_ETC1_UNORM,   GPU_ETC1
);

constexpr auto blendEquations = BidirectionalMap<>::Create(
    RenderState::BLENDOP_ADD,                GPU_BLEND_ADD,
    RenderState::BLENDOP_SUBTRACT,           GPU_BLEND_SUBTRACT,
    RenderState::BLENDOP_REVERSE_SUBTRACT,   GPU_BLEND_REVERSE_SUBTRACT,
    RenderState::BLENDOP_MIN,                GPU_BLEND_MIN,
    RenderState::BLENDOP_MAX,                GPU_BLEND_MAX
);

constexpr auto blendFactors = BidirectionalMap<>::Create(
    RenderState::BLENDFACTOR_ZERO,                GPU_ZERO,
    RenderState::BLENDFACTOR_ONE,                 GPU_ONE,
    RenderState::BLENDFACTOR_SRC_COLOR,           GPU_SRC_COLOR,
    RenderState::BLENDFACTOR_ONE_MINUS_SRC_COLOR, GPU_ONE_MINUS_SRC_COLOR,
    RenderState::BLENDFACTOR_SRC_ALPHA,           GPU_SRC_ALPHA,
    RenderState::BLENDFACTOR_ONE_MINUS_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA,
    RenderState::BLENDFACTOR_DST_COLOR,           GPU_DST_COLOR,
    RenderState::BLENDFACTOR_ONE_MINUS_DST_COLOR, GPU_ONE_MINUS_DST_COLOR,
    RenderState::BLENDFACTOR_DST_ALPHA,           GPU_DST_ALPHA,
    RenderState::BLENDFACTOR_ONE_MINUS_DST_ALPHA, GPU_ONE_MINUS_DST_ALPHA,
    RenderState::BLENDFACTOR_SRC_ALPHA_SATURATED, GPU_SRC_ALPHA_SATURATE
);

constexpr auto filterModes = BidirectionalMap<>::Create(
    SamplerState::FILTER_LINEAR,  GPU_LINEAR,
    SamplerState::FILTER_NEAREST, GPU_NEAREST
);

constexpr auto wrapModes = BidirectionalMap<>::Create(
    SamplerState::WRAP_CLAMP,           GPU_CLAMP_TO_EDGE,
    SamplerState::WRAP_CLAMP_ZERO,      GPU_CLAMP_TO_BORDER,
    SamplerState::WRAP_REPEAT,          GPU_REPEAT,
    SamplerState::WRAP_MIRRORED_REPEAT, GPU_MIRRORED_REPEAT
);

constexpr auto cullModes = BidirectionalMap<>::Create(
    Vertex::CULL_NONE,  GPU_CULL_NONE,
    Vertex::CULL_BACK,  GPU_CULL_BACK_CCW,
    Vertex::CULL_FRONT, GPU_CULL_FRONT_CCW
);

constexpr auto compareModes = BidirectionalMap<>::Create(
    RenderState::COMPARE_LESS,     GPU_LESS,
    RenderState::COMPARE_LEQUAL,   GPU_LEQUAL,
    RenderState::COMPARE_EQUAL,    GPU_EQUAL,
    RenderState::COMPARE_GEQUAL,   GPU_GEQUAL,
    RenderState::COMPARE_GREATER,  GPU_GREATER,
    RenderState::COMPARE_NOTEQUAL, GPU_NOTEQUAL,
    RenderState::COMPARE_ALWAYS,   GPU_ALWAYS,
    RenderState::COMPARE_NEVER,    GPU_NEVER
);
// clang-format on

bool citro2d::GetConstant(PixelFormat in, GPU_TEXCOLOR& out)
{
    return pixelFormats.Find(in, out);
}

bool citro2d::GetConstant(GPU_TEXCOLOR in, PixelFormat& out)
{
    return pixelFormats.ReverseFind(in, out);
}

bool citro2d::GetConstant(RenderState::BlendOperation in, GPU_BLENDEQUATION& out)
{
    return blendEquations.Find(in, out);
}

bool citro2d::GetConstant(RenderState::BlendFactor in, GPU_BLENDFACTOR& out)
{
    return blendFactors.Find(in, out);
}

bool citro2d::GetConstant(SamplerState::FilterMode in, GPU_TEXTURE_FILTER_PARAM& out)
{
    return filterModes.Find(in, out);
}

bool citro2d::GetConstant(SamplerState::WrapMode in, GPU_TEXTURE_WRAP_PARAM& out)
{
    return wrapModes.Find(in, out);
}

bool citro2d::GetConstant(Vertex::CullMode in, GPU_CULLMODE& out)
{
    return cullModes.Find(in, out);
}

bool citro2d::GetConstant(RenderState::CompareMode in, GPU_TESTFUNC& out)
{
    return compareModes.Find(in, out);
}
