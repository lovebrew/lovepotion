#pragma once

#include "modules/window/Window.tcc"

namespace love
{
    class Window final : public WindowBase<Window>
    {
      public:
        Window();

        virtual ~Window();

        bool setWindow(int width = 800, int height = 600, WindowSettings* settings = nullptr);

        void getWindow(int& width, int& height, WindowSettings& settings);

        void close();

        bool onSizeChanged(int widht, int height);

        int getDisplayCount() const;

        std::string_view getDisplayName(int displayIndex) const;

        DisplayOrientation getDisplayOrientation(int displayIndex) const;

        std::vector<WindowSize> getFullscreenSizes(int displayIndex) const;

        void getDesktopDimensions(int displayIndex, int& width, int& height) const;

        bool isOpen() const
        {
            return this->open;
        }

        void setDisplaySleepEnabled(bool enable);

        bool isDisplaySleepEnabled() const;

      private:
        void close(bool allowExceptions);

        bool createWindowAndContext(int x, int y, int width, int height, uint32_t flags);

        WindowSettings settings;
        bool open;

        int width;
        int height;

        int pixelWidth;
        int pixelHeight;
    };
} // namespace love
