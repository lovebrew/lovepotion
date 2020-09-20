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

            Renderer * GetRenderer();

            // Löve2D Functions

            int GetDisplayCount();

            std::vector<std::pair<int, int>> & GetFullscreenModes();

            bool IsOpen();

            bool SetMode();

            void SetScreen(size_t screen);

            void Clear(Color * color);

            // void Clear(const Graphics::DisplayState & state);

            void Present();

            // End Löve2D Functions

            void SetRenderer(Canvas * renderer);

            void SetRenderer(TextureHandle handle);

            void SetGraphics(Graphics * g);

            int GetDisplay() {
                return this->currentDisplay;
            };

        private:
            std::vector<std::pair<int, int>> displaySizes;
            std::vector<Renderer *> targets;

            StrongReference<Canvas> canvas;
            bool hasCanvas;

            Frame * window;
            bool open;
            StrongReference<Graphics> graphics;

            int currentDisplay = 0;
    };
}
