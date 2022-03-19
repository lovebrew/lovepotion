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

        static constexpr int TOP_WIDTH    = 0x190;
        static constexpr int BOTTOM_WIDTH = 0x140;
        static constexpr int HEIGHT       = 0x0F0;

        enum class CtrScreen : RenderScreen
        {
            CTR_SCREEN_LEFT,
            CTR_SCREEN_RIGHT,
            CTR_SCREEN_BOTTOM,
            CTR_SCREEN_MAX_ENUM
        };

        enum class Ctr2dScreen : RenderScreen
        {
            CTR_2D_SCREEN_TOP,
            CTR_2D_SCREEN_BOTTOM,
            CTR_2D_SCREEN_MAX_ENUM
        };

        int GetWidth(RenderScreen screen) override;

        int GetHeight(RenderScreen screen) override;

        static bool GetConstant(const char* in, RenderScreen& out);
        static bool GetConstant(RenderScreen in, const char*& out);
        static std::vector<const char*> GetConstants(RenderScreen);
    }
} // namespace love
