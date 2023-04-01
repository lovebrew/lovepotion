#include <utilities/driver/renderer_ext.hpp>

#include <common/exception.hpp>
#include <common/luax.hpp>

#include <objects/texture_ext.hpp>

using namespace love;

Renderer<Console::CTR>::Renderer() : targets { nullptr }
{
    gfxInitDefault();
    gfxSet3D(true);

    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
        throw love::Exception("Failed to initialize citro3d!");

    if (!C2D_Init(C2D_DEFAULT_MAX_OBJECTS))
        throw love::Exception("Failed to initialize citro2d!");

    C2D_Prepare();

    C2D_Flush();
    C3D_AlphaTest(true, GPU_GREATER, 0);
    C2D_SetTintMode(C2D_TintMult);
}

Renderer<Console::CTR>::~Renderer()
{
    C2D_Fini();

    C3D_Fini();

    gfxExit();
}

Renderer<Console::CTR>::Info Renderer<Console::CTR>::GetRendererInfo()
{
    if (this->info.filled)
        return this->info;

    this->info.device  = Renderer::RENDERER_DEVICE;
    this->info.name    = Renderer::RENDERER_NAME;
    this->info.vendor  = Renderer::RENDERER_VENDOR;
    this->info.version = Renderer::RENDERER_VERSION;

    this->info.filled = true;

    return this->info;
}

void Renderer<Console::CTR>::CreateFramebuffers()
{

    this->targets = { C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT),
                      C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT),
                      C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT) };
}

void Renderer<Console::CTR>::DestroyFramebuffers()
{
    for (auto* framebuffer : this->targets)
        C3D_RenderTargetDelete(framebuffer);
}

void Renderer<Console::CTR>::Clear(const Color& color)
{
    C2D_TargetClear(this->current, color.rgba());
}

/* todo */
void Renderer<Console::CTR>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{}

/* kept track of in Graphics<Console::CTR> */
void Renderer<Console::CTR>::SetBlendColor(const Color& color)
{}

void Renderer<Console::CTR>::EnsureInFrame()
{
    if (!this->inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        this->inFrame = true;
    }
}

void Renderer<Console::CTR>::BindFramebuffer()
{
    if (!IsActiveScreenValid())
        return;

    this->EnsureInFrame();

    this->current = this->targets[love::GetActiveScreen()];
    C2D_SceneBegin(this->current);
}

void Renderer<Console::CTR>::Present()
{
    if (this->inFrame)
    {
        C3D_FrameEnd(0);
        this->inFrame = false;
    }

    for (size_t i = this->deferred.size(); i > 0; i--)
    {
        this->deferred[i - 1]();
        this->deferred.erase(deferred.begin() + i - 1);
    }
}

void Renderer<Console::CTR>::SetViewport(const Rect& viewport)
{
    C2D_Flush();
    C3D_SetViewport(viewport.x, viewport.y, viewport.w, viewport.h);

    this->viewport = viewport;
}

void Renderer<Console::CTR>::SetScissor(const Rect& scissor, bool canvasActive)
{
    const bool enable    = (scissor == Rect::EMPTY);
    GPU_SCISSORMODE mode = enable ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

    if (enable)
        C2D_Flush();

    auto* graphics = Module::GetInstance<Graphics<Console::CTR>>(Module::M_GRAPHICS);

    if (!graphics)
        throw love::Exception("Graphics module not loaded.");

    const auto width = love::GetScreenWidth();

    uint32_t left   = 240 > (scissor.y + scissor.h) ? 240 - (scissor.y + scissor.h) : 0;
    uint32_t top    = width > (scissor.x + scissor.w) ? width - (scissor.x + scissor.w) : 0;
    uint32_t right  = 240 - scissor.y;
    uint32_t bottom = width - scissor.x;

    C3D_SetScissor(mode, left, top, right, bottom);
}

void Renderer<Console::CTR>::SetStencil(RenderState::CompareMode mode, int value)
{
    bool enabled = (mode == RenderState::COMPARE_ALWAYS) ? false : true;

    std::optional<GPU_TESTFUNC> compareOp;
    if (!(compareOp = Renderer::compareModes.Find(mode)))
        return;

    C3D_StencilTest(enabled, *compareOp, value, 0xFFFFFFFF, 0xFFFFFFFF);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
}

void Renderer<Console::CTR>::SetMeshCullMode(vertex::CullMode mode)
{
    std::optional<GPU_CULLMODE> cullMode;
    if (!(cullMode = Renderer::cullModes.Find(mode)))
        return;

    C2D_Flush();
    C3D_CullFace(*cullMode);
}

/* ??? */
void Renderer<Console::CTR>::SetVertexWinding(vertex::Winding winding)
{}

void Renderer<Console::CTR>::SetSamplerState(Texture<Console::CTR>* texture, SamplerState& state)
{
    /* set the min and mag filters */

    auto* handle = texture->GetHandle();

    std::optional<GPU_TEXTURE_FILTER_PARAM> mag;
    if (!(mag = Renderer::filterModes.Find(state.magFilter)))
        return;

    std::optional<GPU_TEXTURE_FILTER_PARAM> min;
    if (!(min = Renderer::filterModes.Find(state.minFilter)))
        return;

    C3D_TexSetFilter(handle, *mag, *min);

    /* set the wrapping modes */

    std::optional<GPU_TEXTURE_WRAP_PARAM> wrapU;
    if (!(wrapU = Renderer::wrapModes.Find(state.wrapU)))
        return;

    std::optional<GPU_TEXTURE_WRAP_PARAM> wrapV;
    if (!(wrapV = Renderer::wrapModes.Find(state.wrapV)))
        return;

    C3D_TexSetWrap(handle, *wrapU, *wrapV);
}

void Renderer<Console::CTR>::SetColorMask(const RenderState::ColorMask& mask)
{
    C2D_Flush();

    uint8_t writeMask = GPU_WRITE_DEPTH;
    writeMask |= mask.GetColorMask();

    C3D_DepthTest(true, GPU_GEQUAL, (GPU_WRITEMASK)writeMask);
}

void Renderer<Console::CTR>::SetBlendMode(const RenderState::BlendState& state)
{
    std::optional<GPU_BLENDEQUATION> opRGB;
    if (!(opRGB = Renderer::blendEquations.Find(state.operationRGB)))
        return;

    std::optional<GPU_BLENDEQUATION> opAlpha;
    if (!(opAlpha = Renderer::blendEquations.Find(state.operationA)))
        return;

    std::optional<GPU_BLENDFACTOR> srcColor;
    if (!(srcColor = Renderer::blendFactors.Find(state.srcFactorRGB)))
        return;

    std::optional<GPU_BLENDFACTOR> dstColor;
    if (!(dstColor = Renderer::blendFactors.Find(state.dstFactorRGB)))
        return;

    std::optional<GPU_BLENDFACTOR> srcAlpha;
    if (!(srcAlpha = Renderer::blendFactors.Find(state.srcFactorA)))
        return;

    std::optional<GPU_BLENDFACTOR> dstAlpha;
    if (!(dstAlpha = Renderer::blendFactors.Find(state.dstFactorA)))
        return;

    C2D_Flush();
    C3D_AlphaBlend(*opRGB, *opAlpha, *srcColor, *dstColor, *srcAlpha, *dstAlpha);
}
