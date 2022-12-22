#include <modules/window_ext.hpp>

#include <modules/graphics_ext.hpp>

using namespace love;

Window<Console::HAC>::Window()
{
    this->sleepAllowed = this->IsDisplaySleepEnabled();
}

Window<Console::HAC>::~Window()
{
    this->Close();
    this->SetDisplaySleepEnabled(this->sleepAllowed);

    this->graphics.Set(nullptr);
}

bool Window<Console::HAC>::SetWindow(int width, int height, WindowSettings* settings)
{
    if (!this->graphics.Get())
        this->graphics.Set((Module::GetInstance<Graphics<Console::HAC>>(Module::M_GRAPHICS)));

    bool setMode = false;

    /* handled internally */
    if (!this->IsOpen())
    {
        if (!this->CreateWindowAndContext(width, height, width, height))
            return false;

        setMode = true;
    }

    if (this->graphics.Get())
    {
        if (setMode)
            this->graphics->SetMode(width, height, width, height);
        else
            this->graphics->SetViewportSize(width, height);
    }

    return true;
}

bool Window<Console::HAC>::CreateWindowAndContext(int x, int y, int width, int height)
{
    this->open = true;
    return true;
}

void Window<Console::HAC>::GetWindow(int& width, int& height, WindowSettings& settings)
{
    width = 1280, height = 720;
    if (appletGetOperationMode() == AppletOperationMode_Console)
        width = 1920, height = 1080;
}

void Window<Console::HAC>::Close()
{
    this->open = false;
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
    appletSetAutoSleepDisabled(!enabled);
}

bool Window<Console::HAC>::IsDisplaySleepEnabled() const
{
    bool sleepDisabled = false;
    appletIsAutoSleepDisabled(&sleepDisabled);

    return !sleepDisabled;
}
