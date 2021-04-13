#pragma once

#include "modules/graphics/graphics.h"

namespace love
{
    class Window : public Module
    {
      public:
        struct DisplaySize
        {
            int width;
            int height;
        };

        Window();

        virtual ~Window();

        ModuleType GetModuleType() const
        {
            return M_WINDOW;
        }

        const char* GetName() const override
        {
            return "love.window";
        }

        // Löve2D Functions

        int GetDisplayCount();

        const std::array<DisplaySize, 2>& GetFullscreenModes();

        bool IsOpen();

        bool SetMode();

        // End Löve2D Functions

        void SetGraphics(Graphics* g);

        void OnSizeChanged(int width, int height);

      private:
        StrongReference<Graphics> graphics;

        bool open;
    };
} // namespace love
