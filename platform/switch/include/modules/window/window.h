#pragma once

#include "modules/window/windowc.h"

namespace love
{
    class Window : public common::Window
    {
      public:
        Window();

        virtual ~Window()
        {}

        void OnSizeChanged(int width, int height) override;

        bool CreateWindowAndContext() override;

        DisplaySize GetDesktopSize() override;

        void GetWindow(int& width, int& height) override;

        int GetDisplayCount() override;
    };
} // namespace love
