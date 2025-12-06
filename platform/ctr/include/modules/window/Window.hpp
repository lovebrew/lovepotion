#pragma once

#include "common/StrongRef.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/window/Window.tcc"

namespace love
{
    class Window final : public WindowBase
    {
      public:
        struct ThemeEntry
        {
            uint32_t index;
            uint8_t dlc_tid_low_bits;
            uint8_t type;
            uint16_t unknown;
        };

        using ArchivePair = std::pair<CFG_Region, uint32_t>;

        Window();

        virtual ~Window();

        void setGraphics(GraphicsBase* graphics)
        {
            this->graphics.set(graphics);
        }

        void close() override;

        bool setWindow(int width = 800, int height = 600, WindowSettings* settings = nullptr) override;

        void updateSettings(const WindowSettings& settings, bool updateGraphicsViewport) override;

        bool onSizeChanged(int width, int height) override;

        void setDisplaySleepEnabled(bool enable) override;

        bool isDisplaySleepEnabled() const override;

        SystemTheme getSystemTheme() const override;

        using WindowBase::getConstant;

        static constexpr std::array<ArchivePair, 4> ThemeArchives = { {
            { CFG_REGION_JPN, 0x00000082 },
            { CFG_REGION_USA, 0x0000008F },
            { CFG_REGION_EUR, 0x00000098 },
            { CFG_REGION_KOR, 0x000000A9 },
        } };

      private:
        void close(bool allowExceptions);

        StrongRef<GraphicsBase> graphics;
    };
} // namespace love
