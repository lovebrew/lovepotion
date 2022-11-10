#include <modules/window_ext.hpp>

#include <modules/graphics_ext.hpp>
#include <utilities/driver/hid_ext.hpp>

#include <3ds.h>

using namespace love;

Window<Console::CTR>::~Window()
{
    this->Close();
    this->graphics.Set(nullptr);
}

bool Window<Console::CTR>::SetWindow(int width, int height, WindowSettings* settings)
{
    if (!this->graphics.Get())
        this->graphics.Set((Module::GetInstance<Graphics<Console::CTR>>(Module::M_GRAPHICS)));

    this->Close();

    /* handled internally */
    if (!this->CreateWindowAndContext(0, 0, 0, 0))
        return false;

    if (this->graphics.Get())
        this->graphics->SetMode(400, 240, 400, 240);

    return true;
}

bool Window<Console::CTR>::CreateWindowAndContext(int x, int y, int width, int height)
{
    this->open = true;
    return true;
}

void Window<Console::CTR>::GetWindow(int& width, int& height, WindowSettings& settings)
{}

void Window<Console::CTR>::Close()
{
    this->open = false;
}

bool Window<Console::CTR>::OnSizeChanged(int width, int height)
{
    return true;
}

std::string_view Window<Console::CTR>::GetDisplayName(int displayIndex) const
{
    switch (displayIndex)
    {
        case 0:
            return "left";
        case 1:
            return "right";
        case 2:
            return "bottom";
        default:
            break;
    }

    return std::string_view {};
}

std::vector<Window<>::WindowSize> Window<Console::CTR>::GetFullscreenSizes(int displayIndex)
{
    return {};
}

void Window<Console::CTR>::GetDesktopDimensions(int displayIndex, int& width, int& height)
{}

void Window<Console::CTR>::SetPosition(int x, int y, int displayIndex)
{}

void Window<Console::CTR>::GetPosition(int& x, int& y, int& displayIndex)
{}

void Window<Console::CTR>::SetDisplaySleepEnabled(bool enabled)
{
    aptSetSleepAllowed(enabled);
}

bool Window<Console::CTR>::IsDisplaySleepEnabled() const
{
    return aptIsSleepAllowed();
}
