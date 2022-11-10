#include <modules/window_ext.hpp>

#include <modules/graphics_ext.hpp>

using namespace love;

Window<Console::HAC>::Window()
{
    this->sleepDisabledDefault = this->IsDisplaySleepEnabled();
}

Window<Console::HAC>::~Window()
{
    this->Close();
    this->SetDisplaySleepEnabled(this->sleepDisabledDefault);
}

bool Window<Console::HAC>::SetWindow(int width, int height, WindowSettings* settings)
{
    if (!this->graphics.Get())
        this->graphics.Set((Module::GetInstance<Graphics<Console::HAC>>(Module::M_GRAPHICS)));

    this->Close();

    /* handled internally */
    if (!this->CreateWindowAndContext(0, 0, 0, 0))
        return false;

    if (this->graphics.Get())
        this->graphics->SetMode(400, 240, 400, 240);

    return true;
}

bool Window<Console::HAC>::CreateWindowAndContext(int x, int y, int width, int height)
{
    this->open = true;
    return true;
}

void Window<Console::HAC>::GetWindow(int& width, int& height, WindowSettings& settings)
{
    int width = 1280, height = 720;
    if (appletGetOperationMode() == AppletOperationMode_Console)
        width = 1920, height = 1080;
}

void Window<Console::HAC>::Close()
{
    this->open = false;
    this->graphics.Set(nullptr);
}

bool Window<Console::HAC>::OnSizeChanged(int width, int height)
{
    return true;
}

std::string_view Window<Console::HAC>::GetDisplayName(int displayIndex) const
{
    return "default";
}

std::vector<Window<>::WindowSize> Window<Console::HAC>::GetFullscreenSizes(int displayIndex)
{
    return {};
}

void Window<Console::HAC>::GetDesktopDimensions(int displayIndex, int& width, int& height)
{}

void Window<Console::HAC>::SetPosition(int x, int y, int displayIndex)
{}

void Window<Console::HAC>::GetPosition(int& x, int& y, int& displayIndex)
{}

void Window<Console::HAC>::SetDisplaySleepEnabled(bool enabled)
{
    appletSetAutoSleepDisabled(enabled);
}

bool Window<Console::HAC>::IsDisplaySleepEnabled() const
{
    bool sleepDisabled = false;
    appletIsAutoSleepDisabled(&sleepDisabled);

    return !sleepDisabled;
}
