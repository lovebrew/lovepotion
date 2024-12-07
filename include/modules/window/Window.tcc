#pragma once

#include "common/Module.hpp"
#include "common/Optional.hpp"
#include "common/math.hpp"

#include "common/Map.hpp"
#include "common/screen.hpp"

namespace love
{
    static inline bool highDPIAllowed = false;

    inline void setHighDPIAllowed(bool allow)
    {
        highDPIAllowed = allow;
    }

    inline bool isHighDPIAllowed()
    {
        return highDPIAllowed;
    }

    template<class T>
    class WindowBase : public Module
    {
      public:
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

        enum FullscreenType
        {
            FULLSCREEN_EXCLUSIVE,
            FULLSCREEN_DESKTOP,
            FULLSCREEN_WINDOWED,
            FULLSCREEN_MAX_ENUM
        };

        enum MessageBoxType
        {
            MESSAGEBOX_ERROR,
            MESSAGEBOX_WARNING,
            MESSAGEBOX_INFO,
            MESSAGEBOX_MAX_ENUM
        };

        enum DisplayOrientation
        {
            ORIENTATION_UNKNOWN,
            ORIENTATION_LANDSCAPE,
            ORIENTATION_LANDSCAPE_FLIPPED,
            ORIENTATION_PORTRAIT,
            ORIENTATION_PORTRAIT_FLIPPED,
            ORIENTATION_MAX_ENUM
        };

        struct WindowSize
        {
            int width;
            int height;

            bool operator==(const WindowSize& w) const
            {
                return (width == w.width) && (height == w.height);
            }
        };

        struct WindowSettings
        {
            bool fullscreen       = false;
            FullscreenType fsType = FullscreenType::FULLSCREEN_DESKTOP;
            int vsync             = 1;
            int msaa              = 0;
            bool stencil          = true;
            bool depth            = false;
            bool resizable        = false;
            int minwidth          = 1;
            int minheight         = 1;
            bool borderless       = false;
            bool centered         = true;
            int displayindex      = 0;
            bool usedpiscale      = true;
            double refreshrate    = 0.0;
            bool useposition      = false;
            int x                 = 0;
            int y                 = 0;
        };

        struct MessageBoxData
        {
            MessageBoxType type;

            std::string title;
            std::string message;

            std::vector<std::string> buttons;
            int enterButtonIndex;
            int escapeButtonIndex;

            bool attachToWindow;
        };

        WindowBase(const char* name) :
            Module(M_WINDOW, name),
            pixelWidth(0),
            pixelHeight(0),
            windowWidth(0),
            windowHeight(0),
            open(false)
        {}

        virtual ~WindowBase()
        {}

        bool isOpen() const
        {
            return this->open;
        }

        void updateSettings(const WindowSettings& newSettings, bool updateGraphicsViewport)
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

            static_cast<T*>(this)->updateSettingsImpl(newSettings, updateGraphicsViewport);
        }

        void getWindow(int& width, int& height, WindowSettings& newSettings)
        {
            width       = this->windowWidth;
            height      = this->windowHeight;
            newSettings = this->settings;
        }

        bool setFullscreen(bool fullscreen, FullscreenType fstype)
        {
            WindowSettings newSettings = settings;
            newSettings.fullscreen     = fullscreen;
            newSettings.fsType         = fstype;

            this->updateSettings(newSettings, true);

            return true;
        }

        bool setFullscreen(bool fullscreen)
        {
            return this->setFullscreen(fullscreen, this->settings.fsType);
        }

        int getDisplayCount() const
        {
            return love::getScreenInfo().size();
        }

        std::string_view getDisplayName(int displayIndex) const
        {
            const auto& info = love::getScreenInfo((Screen)displayIndex);

            return info.name;
        }

        std::vector<WindowSize> getFullscreenSizes(int displayIndex) const
        {
            const auto& info = love::getScreenInfo((Screen)displayIndex);

            return { { info.width, info.height } };
        }

        void getDesktopDimensions(int displayIndex, int& width, int& height) const
        {
            const auto result = this->getFullscreenSizes(displayIndex);

            width  = result.back().width;
            height = result.back().height;
        }

        DisplayOrientation getDisplayOrientation(int) const
        {
            return DisplayOrientation::ORIENTATION_LANDSCAPE;
        }

        double getDPIScale() const
        {
            return 1.0;
        }

        double getNativeDPIScale() const
        {
            return 1.0;
        }

        double toPixels(double x) const
        {
            return x * this->getDPIScale();
        }

        void toPixels(double x, double y, double& pixelx, double& pixely) const
        {
            double scale = this->getDPIScale();

            pixelx = x * scale;
            pixely = y * scale;
        }

        void fromPixels(int pixelx, int pixely, double& x, double& y) const
        {
            double scale = this->getDPIScale();

            x = pixelx / scale;
            y = pixely / scale;
        }

        double fromPixels(int pixels) const
        {
            return pixels / this->getDPIScale();
        }

        // clang-format off
        STRINGMAP_DECLARE(Settings, Setting,
            { "fullscreen",     SETTING_FULLSCREEN      },
            { "fullscreentype", SETTING_FULLSCREEN_TYPE },
            { "vsync",          SETTING_VSYNC           },
            { "msaa",           SETTING_MSAA            },
            { "stencil",        SETTING_STENCIL         },
            { "depth",          SETTING_DEPTH           },
            { "resizable",      SETTING_RESIZABLE       },
            { "minwidth",       SETTING_MIN_WIDTH       },
            { "minheight",      SETTING_MIN_HEIGHT      },
            { "borderless",     SETTING_BORDERLESS      },
            { "centered",       SETTING_CENTERED        },
            { "displayindex",   SETTING_DISPLAYINDEX    },
            { "usedpiscale",    SETTING_USE_DPISCALE    },
            { "refreshrate",    SETTING_REFRESHRATE     },
            { "x",              SETTING_X               },
            { "y",              SETTING_Y               }
        );

        STRINGMAP_DECLARE(FullscreenTypes, FullscreenType,
            { "exclusive", FULLSCREEN_EXCLUSIVE },
            { "desktop",   FULLSCREEN_DESKTOP   }
        );

        STRINGMAP_DECLARE(MessageBoxTypes, MessageBoxType,
            { "error",   MESSAGEBOX_ERROR   },
            { "warning", MESSAGEBOX_WARNING },
            { "info",    MESSAGEBOX_INFO    }
        );

        STRINGMAP_DECLARE(DisplayOrientations, DisplayOrientation,
            { "unknown",          ORIENTATION_UNKNOWN           },
            { "landscape",        ORIENTATION_LANDSCAPE         },
            { "landscapeflipped", ORIENTATION_LANDSCAPE_FLIPPED },
            { "portrait",         ORIENTATION_PORTRAIT          },
            { "portraitflipped",  ORIENTATION_PORTRAIT_FLIPPED  }
        );
        // clang-format on

      protected:
        bool createWindowAndContext(int x, int y, int width, int height, uint32_t flags)
        {
            this->open = true;
            return true;
        }

        int pixelWidth;
        int pixelHeight;

        int windowWidth;
        int windowHeight;

        bool open;

        WindowSettings settings;
    };
} // namespace love
