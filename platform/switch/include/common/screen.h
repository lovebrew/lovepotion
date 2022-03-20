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

        static constexpr int HANDHELD_WIDTH  = 0x500;
        static constexpr int HANDHELD_HEIGHT = 0x2D0;

        static constexpr int DOCKED_WIDTH  = 0x780;
        static constexpr int DOCKED_HEIGHT = 0x438;

        enum class HacScreen : RenderScreen
        {
            HAC_SCREEN_DEFAULT,
            HAC_SCREEN_MAX_ENUM
        };

        int GetWidth(RenderScreen screen = 0) override;

        int GetHeight() override;

        static bool GetConstant(const char* in, RenderScreen& out);
        static bool GetConstant(RenderScreen in, const char*& out);
        static std::vector<const char*> GetConstants(RenderScreen);
    };
} // namespace love
