#include "common/screen.hpp"

#include "modules/window/Window.hpp"

#include <3ds.h>

namespace love
{
    Window::Window() : WindowBase("love.window.citro3d")
    {
        this->setDisplaySleepEnabled(false);
    }

    Window::~Window()
    {
        this->close(false);
        this->graphics.set(nullptr);
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

        this->close();

        if (!this->createWindowAndContext(0, 0, width, height, 0))
            return false;

        if (this->graphics.get())
            this->graphics->setMode(width, height, width, height, false, false, 0);

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
        return true;
    }

    void Window::setDisplaySleepEnabled(bool enable)
    {
        aptSetSleepAllowed(enable);
    }

    bool Window::isDisplaySleepEnabled() const
    {
        return aptIsSleepAllowed();
    }

    Window::SystemTheme Window::getSystemTheme() const
    {
        uint8_t region = 0;
        uint32_t id    = 0;

        CFGU_SecureInfoGetRegion(&region);
        auto it = std::ranges::find(Window::ThemeArchives, region, &ArchivePair::first);
        if (it != Window::ThemeArchives.end())
            id = it->second;

        Result result;
        const uint32_t path[3] = { MEDIATYPE_SD, id, 0 };
        FS_Path home { PATH_BINARY, 0xC, path };

        FS_Archive archive;
        if (R_FAILED(result = FSUSER_OpenArchive(&archive, ARCHIVE_EXTDATA, home)))
            return THEME_UNKNOWN;

        Handle file         = 0;
        const auto filepath = fsMakePath(PATH_ASCII, "/SaveData.dat");
        if (R_FAILED(result = FSUSER_OpenFile(&file, archive, filepath, FS_OPEN_READ, 0)))
            return THEME_UNKNOWN;

        Window::ThemeEntry entry {};

        if (R_FAILED(result = FSFILE_Read(file, nullptr, 0x13B8, &entry, sizeof(ThemeEntry))))
        {
            FSFILE_Close(file);
            return THEME_UNKNOWN;
        }

        FSFILE_Close(file);

        if (entry.type == 2) // DLC Theme
            return THEME_CUSTOM;
        else if (entry.type == 0) // "No" Theme
            return THEME_LIGHT;

        // Basic color themes, considered "dark" themes here.
        return THEME_DARK;
    }
} // namespace love
