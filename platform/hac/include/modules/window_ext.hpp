#pragma once

#include <modules/window/window.tcc>

#include <modules/graphics_ext.hpp>

#include <switch.h>

namespace love
{
    template<>
    class Window<Console::HAC> : public Window<Console::ALL>
    {
      public:
        Window();

        virtual ~Window();

        void SetGraphics(Graphics<Console::CTR>* graphics)
        {
            this->graphics.Set(graphics);
        }

        bool CreateWindowAndContext(int x, int y, int width, int height);

        bool SetWindow(int width = 800, int height = 600, WindowSettings* settings = nullptr);

        void GetWindow(int& width, int& height, WindowSettings& settings);

        void Close();

        bool SetFullscreen(bool fullscreen, FullscreenType fullScreenType)
        {
            return true;
        }

        bool SetFullscreen(bool fullscreen)
        {
            return true;
        }

        bool OnSizeChanged(int width, int height);

        int GetDisplayCount() const
        {
            return 1;
        }

        std::string_view GetDisplayName(int displayIndex) const;

        std::vector<WindowSize> GetFullscreenSizes(int displayIndex);

        void GetDesktopDimensions(int displayIndex, int& width, int& height);

        void SetPosition(int x, int y, int displayIndex);

        void GetPosition(int& x, int& y, int& displayIndex);

        void SetDisplaySleepEnabled(bool enable);

        bool IsDisplaySleepEnabled() const;

      private:
        bool sleepDisabledDefault;
        StrongReference<Graphics<Console::HAC>> graphics;
    };
} // namespace love
