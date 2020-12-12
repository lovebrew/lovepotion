#pragma once

#include "modules/graphics/graphics.h"

namespace love
{
    class Window : public Module
    {
        public:
            Window();
            ~Window();

            ModuleType GetModuleType() const { return M_WINDOW; }

            const char * GetName() const override { return "love.window"; }

            // Löve2D Functions

            int GetDisplayCount();

            std::vector<std::pair<int, int>> & GetFullscreenModes();

            bool IsOpen();

            bool SetMode();

            // End Löve2D Functions

            void SetGraphics(Graphics * g);

        private:
            std::vector<std::pair<int, int>> displaySizes;
            StrongReference<Graphics> graphics;

            bool open;
    };
}
