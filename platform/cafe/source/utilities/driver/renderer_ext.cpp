#include <utilities/driver/renderer_ext.hpp>

#include <modules/keyboard_ext.hpp>
#include <nn/swkbd.h>

using namespace love;

#define Keyboard() (Module::GetInstance<Keyboard<Console::CAFE>>(Module::M_KEYBOARD))

Renderer<Console::CAFE>::Renderer()
{
    WHBGfxInit();
}

Renderer<Console::CAFE>::~Renderer()
{
    WHBGfxShutdown();
}

void Renderer<Console::CAFE>::CreateFramebuffers()
{
    const auto tv = std::make_pair(WHBGfxBeginRenderTV, WHBGfxFinishRenderTV);
    this->rendertargets.emplace(Screen::SCREEN_TV, tv);

    const auto drc = std::make_pair(WHBGfxBeginRenderDRC, WHBGfxFinishRenderDRC);
    this->rendertargets.emplace(Screen::SCREEN_GAMEPAD, drc);

    this->keyboardRender.emplace(Screen::SCREEN_TV, nn::swkbd::DrawTV);
    this->keyboardRender.emplace(Screen::SCREEN_GAMEPAD, nn::swkbd::DrawDRC);
}

void Renderer<Console::CAFE>::DestroyFramebuffers()
{
    this->rendertargets.clear();
}

void Renderer<Console::CAFE>::EnsureInFrame()
{
    if (this->inFrame)
        return;

    WHBGfxBeginRender();
}

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

    this->rendertargets[Graphics<>::activeScreen].first();
}

void Renderer<Console::CAFE>::Present()
{
    if (!this->inFrame)
        return;

    if (Keyboard()->IsShowing())
        this->keyboardRender[Graphics<>::activeScreen]();

    this->rendertargets[Graphics<>::activeScreen].second();
    WHBGfxFinishRender();
}

std::optional<Screen> Renderer<Console::CAFE>::CheckScreen(const char* name) const
{
    return gfxScreens.Find(name);
}

SmallTrivialVector<const char*, 2> Renderer<Console::CAFE>::GetScreens() const
{
    return { gfxScreens.GetNames() };
}
