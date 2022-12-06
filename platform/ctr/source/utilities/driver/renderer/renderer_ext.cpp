#include <utilities/driver/renderer_ext.hpp>

#include <common/exception.hpp>
#include <common/luax.hpp>

#include <objects/texture_ext.hpp>

#include <modules/graphics_ext.hpp>

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

std::optional<Screen> Renderer<Console::CTR>::CheckScreen(const char* name) const
{
    if (this->Get3D())
        return gfx3dScreens.Find(name);

    return gfx2dScreens.Find(name);
}

SmallTrivialVector<const char*, 3> Renderer<Console::CTR>::GetScreens() const
{
    if (this->Get3D())
        return gfx3dScreens.GetNames();

    return gfx2dScreens.GetNames();
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
    if ((uint8_t)Graphics<Console::ALL>::activeScreen < 0)
        return;

    this->EnsureInFrame();

    this->current = this->targets[(uint8_t)Graphics<Console::ALL>::activeScreen];
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

void Renderer<Console::CTR>::SetScissor(bool enable, const Rect& scissor, bool canvasActive)
{
    GPU_SCISSORMODE mode = enable ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

    if (enable)
        C2D_Flush();

    // size_t width = Screen::Instance().GetWidth(Graphics::ActiveScreen);

    uint32_t left = 240 > (scissor.y + scissor.h) ? 240 - (scissor.y + scissor.h) : 0;
    // uint32_t top    = width > (scissor.x + scissor.w) ? width - (scissor.x + scissor.w) : 0;
    uint32_t right = 240 - scissor.y;
    // uint32_t bottom = width - scissor.x;

    // C3D_SetScissor(mode, left, top, right, bottom);
}

/* todo */
void Renderer<Console::CTR>::SetStencil(RenderState::CompareMode mode, int value)
{}

void Renderer<Console::CTR>::SetMeshCullMode(Vertex::CullMode mode)
{}

void Renderer<Console::CTR>::SetVertexWinding(Vertex::Winding winding)
{}

/* todo -- need Texture */
void Renderer<Console::CTR>::SetSamplerState(Texture<Console::CTR>* texture, SamplerState& state)
{}

/* todo */
void Renderer<Console::CTR>::SetColorMask(const RenderState::ColorMask& mask)
{}

/* todo */
void Renderer<Console::CTR>::SetBlendMode(const RenderState::BlendState& state)
{}

/* handled in Graphics<Console::ALL> */
void Renderer<Console::CTR>::SetLineWidth(float width)
{}

void Renderer<Console::CTR>::SetLineStyle(Graphics<Console::Which>::LineStyle style)
{}

/* handled in Graphics<Console::ALL> */
void Renderer<Console::CTR>::SetPointSize(float size)
{}
