#include <modules/window_ext.hpp>

using namespace love;

Window<Console::CAFE>::Window()
{
    this->sleepAllowed = this->IsDisplaySleepEnabled();
}

Window<Console::CAFE>::~Window()
{
    this->Close();

    this->SetDisplaySleepEnabled(this->sleepAllowed);
    this->graphics.Set(nullptr);
}

bool Window<Console::CAFE>::SetWindow(int width, int height, WindowSettings* settings)
{
    if (!this->graphics.Get())
        this->graphics.Set((Module::GetInstance<Graphics<Console::CAFE>>(Module::M_GRAPHICS)));

    this->Close();

    /* handled internally */
    if (!this->CreateWindowAndContext(0, 0, 0, 0))
        return false;

    if (this->graphics.Get())
        this->graphics->SetMode(400, 240, 400, 240);

    return true;
}

bool Window<Console::CAFE>::CreateWindowAndContext(int x, int y, int width, int height)
{
    this->open = true;
    return true;
}

void Window<Console::CAFE>::GetWindow(int& width, int& height, WindowSettings& settings)
{}

void Window<Console::CAFE>::Close()
{
    this->open = false;
}

bool Window<Console::CAFE>::OnSizeChanged(int width, int height)
{
    return true;
}

std::string_view Window<Console::CAFE>::GetDisplayName(int displayIndex) const
{
    switch (displayIndex)
    {
        case 0:
        default:
            return "tv";
        case 1:
            return "gamepad";
    }

    return std::string_view {};
}

std::vector<Window<>::WindowSize> Window<Console::CAFE>::GetFullscreenSizes(int displayIndex)
{
    return {};
}

void Window<Console::CAFE>::GetDesktopDimensions(int displayIndex, int& width, int& height)
{}

void Window<Console::CAFE>::SetPosition(int x, int y, int displayIndex)
{}

void Window<Console::CAFE>::GetPosition(int& x, int& y, int& displayIndex)
{}

void Window<Console::CAFE>::SetDisplaySleepEnabled(bool enabled)
{}

bool Window<Console::CAFE>::IsDisplaySleepEnabled() const
{
    return true;
}
