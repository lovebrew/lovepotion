#include "common/screen.hpp"

#include "modules/window/Window.hpp"

#include <3ds.h>

namespace love
{
    Window::Window() : open(false)
    {
        this->setDisplaySleepEnabled(false);
    }

    Window::~Window()
    {
        this->close(false);
    }

    void Window::close(bool allowExceptions)
    {
        this->open = false;
    }

    void Window::close()
    {
        this->close(true);
    }

    bool Window::createWindowAndContext(int x, int y, int width, int height, uint32_t flags)
    {
        this->open = true;
        return true;
    }

    bool Window::setWindow(int width, int height, WindowSettings* settings)
    {
        return true;
    }

    void Window::getWindow(int& width, int& height, WindowSettings& settings)
    {}

    bool Window::onSizeChanged(int width, int height)
    {
        return false;
    }

    int Window::getDisplayCount() const
    {
        return love::getScreenInfo().size();
    }

    std::string_view Window::getDisplayName(int displayIndex) const
    {
        const auto& info = love::getScreenInfo((Screen)displayIndex);

        return info.name;
    }

    std::vector<Window::WindowSize> Window::getFullscreenSizes(int displayIndex) const
    {
        const auto& info = love::getScreenInfo((Screen)displayIndex);

        return { { info.width, info.height } };
    }

    void Window::getDesktopDimensions(int displayIndex, int& width, int& height) const
    {
        const auto result = this->getFullscreenSizes(displayIndex);

        width  = result.back().width;
        height = result.back().height;
    }

    void Window::setDisplaySleepEnabled(bool enable)
    {
        aptSetSleepAllowed(enable);
    }

    bool Window::isDisplaySleepEnabled() const
    {
        return aptIsSleepAllowed();
    }
} // namespace love
