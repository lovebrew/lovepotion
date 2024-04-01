#pragma once

#include "common/Module.hpp"
#include "common/Optional.hpp"
#include "common/math.hpp"

#include "common/map.hpp"

namespace love
{

    namespace window
    {
        static inline bool highDPIAllowed = false;

        void setHighDPIAllowed(bool allow)
        {
            highDPIAllowed = allow;
        }

        bool isHighDPIAllowed()
        {
            return highDPIAllowed;
        }
    } // namespace window

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

        WindowBase() : Module(M_WINDOW, "love.window")
        {}

        virtual ~WindowBase()
        {}

        // clang-format off
        STRINGMAP_DECLARE(settings, Setting,
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
            { "y",              SETTING_Y               },
        );

        STRINGMAP_DECLARE(fullscreenTypes, FullscreenType,
            { "exclusive", FULLSCREEN_EXCLUSIVE },
            { "desktop",   FULLSCREEN_DESKTOP   }
        );

        STRINGMAP_DECLARE(messageBoxTypes, MessageBoxType,
            { "error",   MESSAGEBOX_ERROR   },
            { "warning", MESSAGEBOX_WARNING },
            { "info",    MESSAGEBOX_INFO    }
        );

        STRINGMAP_DECLARE(displayOrientations, DisplayOrientation,
            { "unknown",          ORIENTATION_UNKNOWN           },
            { "landscape",        ORIENTATION_LANDSCAPE         },
            { "landscapeflipped", ORIENTATION_LANDSCAPE_FLIPPED },
            { "portrait",         ORIENTATION_PORTRAIT          },
            { "portraitflipped",  ORIENTATION_PORTRAIT_FLIPPED  }
        );
        // clang-format on
    };
} // namespace love
