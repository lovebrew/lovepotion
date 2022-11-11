#include <utilities/driver/renderer_ext.hpp>

#include <modules/keyboard_ext.hpp>
#include <nn/swkbd.h>

using namespace love;

#define Keyboard() (Module::GetInstance<Keyboard<Console::CAFE>>(Module::M_KEYBOARD))

Renderer<Console::CAFE>::Renderer()
{}

Renderer<Console::CAFE>::~Renderer()
{}

void Renderer<Console::CAFE>::CreateFramebuffers()
{
    auto tvFuncs = RenderFuncs { WHBGfxBeginRenderTV, WHBGfxFinishRenderTV, nn::swkbd::DrawTV };
    this->rendertargets[(uint8_t)Screen::SCREEN_TV] = tvFuncs;

    auto drcFuncs = RenderFuncs { WHBGfxBeginRenderDRC, WHBGfxFinishRenderDRC, nn::swkbd::DrawDRC };
    this->rendertargets[(uint8_t)Screen::SCREEN_GAMEPAD] = drcFuncs;
}

void Renderer<Console::CAFE>::DestroyFramebuffers()
{}

void Renderer<Console::CAFE>::EnsureInFrame()
{
    if (!this->inFrame)
    {
        WHBGfxBeginRender();
        this->inFrame = true;
    }
}

#include <utilities/log/logfile.h>
void Renderer<Console::CAFE>::Clear(const Color& color)
{
    WHBGfxClearColor(color.r, color.g, color.b, color.a);
}

void Renderer<Console::CAFE>::ClearDepthStencil(int stencil, uint8_t mask, double depth)
{}

void Renderer<Console::CAFE>::SetBlendColor(const Color& color)
{}

void Renderer<Console::CAFE>::BindFramebuffer(/* Canvas* canvas */)
{
    this->EnsureInFrame();

    this->rendertargets[(uint8_t)Graphics<>::activeScreen].begin();
}

void Renderer<Console::CAFE>::Present()
{
    if (this->inFrame)
    {
        if (Keyboard()->IsShowing())
            this->rendertargets[(uint8_t)Graphics<>::activeScreen].keyboard();

        this->rendertargets[(uint8_t)Graphics<>::activeScreen].end();

        if (Graphics<>::activeScreen == Screen::SCREEN_GAMEPAD)
        {
            WHBGfxFinishRender();
            this->inFrame = false;
        }
    }
}

std::optional<Screen> Renderer<Console::CAFE>::CheckScreen(const char* name) const
{
    return gfxScreens.Find(name);
}

SmallTrivialVector<const char*, 2> Renderer<Console::CAFE>::GetScreens() const
{
    return gfxScreens.GetNames();
}
