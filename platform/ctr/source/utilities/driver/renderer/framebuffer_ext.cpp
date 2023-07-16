#include <utilities/driver/framebuffer_ext.hpp>

using namespace love;

Framebuffer<Console::CTR>::Framebuffer() : target(nullptr)
{}

void Framebuffer<Console::CTR>::Create(Screen screen)
{
    this->id = screen;
    Mtx_Identity(&this->modelView);

    switch (screen)
    {
        case Screen::LEFT... Screen::RIGHT:
        {
            const auto side = (screen == Screen::LEFT) ? GFX_LEFT : GFX_RIGHT;
            this->SetSize(400, 240, GFX_TOP, side);
            break;
        }
        case Screen::BOTTOM:
        {
            this->SetSize(320, 240, GFX_BOTTOM, GFX_LEFT);
            break;
        }
        default:
            break; // shouldn't happen
    }
}

void Framebuffer<Console::CTR>::Destroy()
{
    if (this->target)
        C3D_RenderTargetDelete(this->target);

    this->target = nullptr;
}

void Framebuffer<Console::CTR>::SetSize(int width, int height, gfxScreen_t screen, gfx3dSide_t side)
{
    this->target = C3D_RenderTargetCreate(height, width, GPU_RB_RGBA8, GPU_RB_DEPTH16);

    if (this->target)
        C3D_RenderTargetSetOutput(this->target, screen, side, Framebuffer::DISPLAY_FLAGS);

    this->viewport = { 0, 0, width, height };
    this->scissor  = { 0, 0, width, height };

    this->SetViewport();
    this->SetScissor();
}

const Rect Framebuffer<Console::CTR>::CalculateBounds(const Rect& bounds)
{
    // clang-format off
    const uint32_t left   = this->height > (bounds.y + bounds.h) ? this->height - (bounds.y + bounds.h) : 0;
    const uint32_t top    = this->width  > (bounds.x + bounds.w) ? this->width - (bounds.x + bounds.w) : 0;
    const uint32_t right  = this->height - bounds.y;
    const uint32_t bottom = this->width  - bounds.x;
    // clang-format on

    return { (int)left, (int)top, (int)right, (int)bottom };
}

void Framebuffer<Console::CTR>::SetViewport(const Rect& viewport)
{
    Rect newViewport = viewport;
    if (viewport == Rect::EMPTY)
        newViewport = this->viewport;

    Mtx_OrthoTilt(&this->projView, newViewport.x, newViewport.w, newViewport.h, newViewport.y,
                  Z_NEAR, Z_FAR, true);
}

void Framebuffer<Console::CTR>::SetScissor(const Rect& scissor)
{
    const bool enable    = (scissor != Rect::EMPTY);
    GPU_SCISSORMODE mode = enable ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

    Rect newScissor = this->CalculateBounds(scissor);
    if (viewport == Rect::EMPTY)
        newScissor = this->CalculateBounds(this->scissor);

    C3D_SetScissor(mode, newScissor.x, newScissor.y, newScissor.w, newScissor.h);
}

void Framebuffer<Console::CTR>::UseProjection(Shader<Console::CTR>::Uniforms uniforms)
{
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniforms.uLocProjMtx, &this->projView);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uniforms.uLocMdlView, &this->modelView);
}