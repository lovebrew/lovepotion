#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Window : public Module
    {
      public:
        struct WindowSize
        {
            int width;
            int height;

            bool operator==(const WindowSize& other) const
            {
                return other.width == this->width && other.height == this->height;
            }
        };

        enum FullscreenType
        {
            FULLSCREEN_EXCLUSIVE,
            FULLSCREEN_DESKTOP,
            FULLSCREEN_MAX_ENUM
        };

        enum Setting
        {
            SETTING_FULLSCREEN,
            SETTING_FULLSCREEN_TYPE,
            SETTING_VSYNC,
            SETTING_MSAA,
            SETTING_STENCIL,
            SETTING_DEPTH,
            SETTING_RESIZABLE,
            SETTING_MIN_WIDTH,
            SETTING_MIN_HEIGHT,
            SETTING_BORDERLESS,
            SETTING_CENTERED,
            SETTING_DISPLAYINDEX,
            SETTING_USE_DPISCALE,
            SETTING_REFRESHRATE,
            SETTING_X,
            SETTING_Y,
            SETTING_MAX_ENUM
        };

        struct WindowSettings
        {
            bool fullscreen               = false;
            FullscreenType fullScreenType = Window::FULLSCREEN_DESKTOP;
            int vsync                     = 1;
            int msaa                      = 0;
            bool stencil                  = true;
            int depth                     = 0;
            bool resizable                = false;
            int minwidth                  = 1;
            int minheight                 = 1;
            bool borderless               = false;
            bool centered                 = true;
            int displayindex              = 0;
            bool usedpiscale              = true;
            double refreshrate            = 0.0;
            bool useposition              = false;
            int x                         = 0;
            int y                         = 0;
        };

        Window() : width(800), height(600), pixelWidth(800), pixelHeight(600)
        {}

        virtual ~Window();

        virtual ModuleType GetModuleType() const
        {
            return M_WINDOW;
        }

        bool IsOpen() const
        {
            return this->open;
        }

        int GetWidth() const
        {
            return this->width;
        }

        int GetHeight() const
        {
            return this->height;
        }

        int GetPixelWidth() const
        {
            return this->pixelWidth;
        }

        int GetPixelHeight() const
        {
            return this->pixelHeight;
        }

        void WindowToPixelCoords(double* x, double* y) const
        {
            if (x != nullptr)
                *x = (*x) * ((double)this->pixelWidth / (double)this->width);

            if (y != nullptr)
                *y = (*y) * ((double)this->pixelHeight / (double)this->height);
        }

        void PixelToWindowCoords(double* x, double* y) const
        {
            if (x != nullptr)
                *x = (*x) * ((double)this->width / (double)this->pixelWidth);

            if (y != nullptr)
                *y = (*y) * ((double)this->height / (double)this->pixelHeight);
        }

        // clang-format off
        static constexpr BidirectionalMap windowSettings = {
            "fullscreen",     SETTING_FULLSCREEN,
            "fullscreentype", SETTING_FULLSCREEN_TYPE,
            "vsync",          SETTING_VSYNC,
            "msaa",           SETTING_MSAA,
            "stencil",        SETTING_STENCIL,
            "depth",          SETTING_DEPTH,
            "resizable",      SETTING_RESIZABLE,
            "minwidth",       SETTING_MIN_WIDTH,
            "minheight",      SETTING_MIN_HEIGHT,
            "borderless",     SETTING_BORDERLESS,
            "centered",       SETTING_CENTERED,
            "displayindex",   SETTING_DISPLAYINDEX,
            "usedpiscale",    SETTING_USE_DPISCALE,
            "refreshrate",    SETTING_REFRESHRATE,
            "x",              SETTING_X,
            "y",              SETTING_Y
        };

        static constexpr BidirectionalMap fullScreenTypes = {
            "exclusive", FULLSCREEN_EXCLUSIVE,
            "desktop",   FULLSCREEN_DESKTOP
        };
        // clang-format on

      protected:
        bool open;

        WindowSettings settings;

        int width;
        int height;

        int pixelWidth;
        int pixelHeight;
    };
} // namespace love
