#pragma once

#include "common/screenc.h"

namespace love
{
    class Screen : common::Screen
    {
      public:
        static Screen& Instance()
        {
            static Screen screen;
            return screen;
        };

        static constexpr int WIDTH  = 0x500;
        static constexpr int HEIGHT = 0x2D0;

        enum class HacScreen : RenderScreen
        {
            HAC_SCREEN_DEFAULT,
            HAC_SCREEN_MAX_ENUM
        };

        int GetWidth(RenderScreen screen) override;

        int GetHeight(RenderScreen screen) override;

        static bool GetConstant(const char* in, RenderScreen& out);
        static bool GetConstant(RenderScreen in, const char*& out);
        static std::vector<const char*> GetConstants(RenderScreen);
    }
} // namespace love
