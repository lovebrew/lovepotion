#include "modules/window/Window.tcc"

namespace love
{
    void WindowBase::updateSettings(const WindowSettings& newSettings, bool updateGraphicsViewport)
    {
        this->pixelWidth  = windowWidth;
        this->pixelHeight = windowHeight;

        this->settings.fullscreen = newSettings.fullscreen;
        this->settings.fsType     = newSettings.fsType;

        this->settings.vsync = newSettings.vsync;

        this->settings.minwidth  = newSettings.minwidth;
        this->settings.minheight = newSettings.minheight;

        this->settings.resizable  = newSettings.resizable;
        this->settings.borderless = newSettings.borderless;
        this->settings.centered   = newSettings.centered;

        this->settings.usedpiscale = newSettings.usedpiscale;

        this->settings.stencil = newSettings.stencil;
        this->settings.depth   = newSettings.depth;
    }

    void WindowBase::getWindow(int& width, int& height, WindowSettings& newSettings)
    {
        width  = windowWidth;
        height = windowHeight;

        newSettings = settings;
    }

    bool WindowBase::setFullscreen(bool fullscreen, FullscreenType type)
    {
        WindowSettings newSettings = settings;
        newSettings.fullscreen     = fullscreen;
        newSettings.fsType         = type;

        this->updateSettings(newSettings, true);

        return true;
    }

    bool WindowBase::setFullscreen(bool fullscreen)
    {
        return this->setFullscreen(fullscreen, this->settings.fsType);
    }
} // namespace love
