#include "modules/window/windowc.h"

using namespace love::common;

Window::Window() : graphics(nullptr), open(false)
{}

Window::~Window()
{
    this->Close(false);
    this->graphics.Set(nullptr);
}

const std::vector<Window::DisplaySize>& Window::GetFullscreenModes()
{
    return this->fullscreenModes;
}

void Window::Close()
{
    this->Close(true);
}

void Window::Close(bool allowExceptions)
{
    if (this->graphics.Get())
    {
        if (allowExceptions && this->graphics->IsCanvasActive())
            throw love::Exception(
                "love.window.close cannot be called while a Canvas is active in love.graphics.");
    }

    this->open = false;
}

bool Window::SetMode()
{
    if (!this->graphics.Get())
        this->graphics.Set(Module::GetInstance<Graphics>(M_GRAPHICS));

    if (this->graphics.Get() && this->graphics->IsCanvasActive())
        throw love::Exception(
            "love.window.setMode cannot be called while a Canvas is active in love.graphics.");

    this->Close();

    auto desktopSize = this->GetDesktopSize();

    if (!this->CreateWindowAndContext())
        return false;

    if (this->graphics.Get())
        this->graphics->SetMode(desktopSize.first, desktopSize.second);

    this->open = true;

    return this->open;
}

void Window::SetGraphics(Graphics* g)
{
    this->graphics.Set(g);
}

bool Window::IsOpen() const
{
    return this->open;
}
