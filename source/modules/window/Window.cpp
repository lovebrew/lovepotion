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
} // namespace love
