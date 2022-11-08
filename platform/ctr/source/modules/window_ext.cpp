#include <modules/window_ext.hpp>

#include <utilities/driver/hid_ext.hpp>

using namespace love;

Window<Console::CTR>::Window()
{}

Window<Console::CTR>::~Window()
{}

bool Window<Console::CTR>::SetWindow(int width, int height, WindowSettings* settings)
{}

void Window<Console::CTR>::GetWindow(int& width, int& height, WindowSettings& settings)
{}

void Window<Console::CTR>::Close()
{}

bool Window<Console::CTR>::OnSizeChanged(int width, int height)
{}

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
{}
