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

        int GetDisplayCount() override;
    };
} // namespace love
