#include "modules/window/Window.hpp"

#include <switch.h>

namespace love
{
    Window::Window() : WindowBase("love.window.deko3d")
    {
        this->setDisplaySleepEnabled(false);
    }

    Window::~Window()
    {
        this->close(false);
        this->graphics.set(nullptr);
    }

    bool Window::setWindow(int width, int height, WindowSettings* settings)
    {
        if (!this->graphics.get())
            this->graphics.set(Module::getInstance<Graphics>(Module::M_GRAPHICS));

        this->close();

        if (!this->createWindowAndContext(0, 0, width, height, 0))
            return false;

        if (this->graphics.get())
            this->graphics->setMode(width, height, width, height, false, false, 0);

        return true;
    }

    void Window::updateSettingsImpl(const WindowSettings& settings, bool updateGraphicsViewport)
    {
        if (updateGraphicsViewport && this->graphics.get())
        {
            double scaledw, scaledh;
            this->fromPixels(this->pixelWidth, this->pixelHeight, scaledw, scaledh);

            this->graphics->backbufferChanged(0, 0, scaledw, scaledh);
        }
    }

    bool Window::onSizeChanged(int width, int height)
    {
        if (this->graphics)
        {
            double scaledw, scaledh;
            this->fromPixels(width, height, scaledw, scaledh);

            this->graphics->backbufferChanged(0, 0, scaledw, scaledh);
            return true;
        }

        return false;
    }

    void Window::setDisplaySleepEnabled(bool enable)
    {
        appletSetAutoSleepDisabled(!enable);
    }

    bool Window::isDisplaySleepEnabled() const
    {
        bool sleepDisabled = false;
        appletIsAutoSleepDisabled(&sleepDisabled);

        return !sleepDisabled;
    }
} // namespace love
