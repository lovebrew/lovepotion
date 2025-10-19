#include "modules/window/Window.hpp"

#include <switch.h>

namespace love
{
    Window::Window() : WindowBase("love.window.deko3d")
    {}

    Window::~Window()
    {
        this->close(false);
        this->graphics.set(nullptr);
    }

    void Window::setGraphics(GraphicsBase* graphics)
    {
        this->graphics.set(graphics);
    }

    void Window::close()
    {
        this->close(true);
    }

    void Window::close(bool allowExceptions)
    {
        if (this->graphics.get())
        {
            if (allowExceptions && this->graphics->isRenderTargetActive())
                throw love::Exception(E_WINDOW_CLOSING_RENDERTARGET_ACTIVE);

            this->graphics->unsetMode();
        }

        this->open = false;
    }

    bool Window::setWindow(int width, int height, WindowSettings* settings)
    {
        if (!this->graphics.get())
            this->graphics.set(Module::getInstance<GraphicsBase>(Module::M_GRAPHICS));

        if (this->graphics.get() && this->graphics->isRenderTargetActive())
            throw love::Exception("love.window.setMode cannot be called while a render target is active.");

        bool shouldSetMode = false;

        if (!this->isOpen())
        {
            if (!this->createWindowAndContext(0, 0, width, height, 0))
                return false;

            shouldSetMode = true;
        }

        if (this->graphics.get())
        {
            double scaledw, scaledh;
            this->fromPixels((double)this->pixelWidth, (double)this->pixelHeight, scaledw, scaledh);

            if (shouldSetMode)
                this->graphics->setMode(width, height, width, height, false, false, 0);
        }

        return true;
    }

    void Window::updateSettings(const WindowSettings& settings, bool updateGraphicsViewport)
    {
        WindowBase::updateSettings(settings, updateGraphicsViewport);

        if (updateGraphicsViewport && this->graphics.get())
        {
            double scaledw, scaledh;
            this->fromPixels(this->pixelWidth, this->pixelHeight, scaledw, scaledh);

            this->graphics->backbufferChanged(0, 0, scaledw, scaledh);
        }
    }

    bool Window::onSizeChanged(int width, int height)
    {
        if (this->graphics.get())
        {
            double scaledw, scaledh;
            this->fromPixels(width, height, scaledw, scaledh);

            this->graphics->backbufferChanged(0, 0, scaledw, scaledh);
        }

        return true;
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
