#pragma once

#include "modules/graphics/graphics.h"

namespace love::common
{
    class Window : public Module
    {
      public:
        typedef std::pair<int, int> DisplaySize;

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

        void Close();

        void Close(bool allowExceptions);

        // Löve2D Functions

        virtual int GetDisplayCount() = 0;

        const std::vector<DisplaySize>& GetFullscreenModes();

        virtual DisplaySize GetDesktopSize()
        {
            DisplaySize size { 0, 0 };
            return size;
        }

        virtual void OnSizeChanged(int width, int height)
        {}

        virtual void GetWindow(int& width, int& height)
        {}

        virtual bool CreateWindowAndContext()
        {
            return true;
        }

        bool IsOpen() const;

        bool SetMode();

        // End Löve2D Functions

        void SetGraphics(Graphics* g);

      protected:
        StrongReference<Graphics> graphics;
        bool open;

        std::vector<DisplaySize> fullscreenModes;
    };
} // namespace love::common
