#pragma once

#include "common/StrongRef.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/window/Window.tcc"

namespace love
{
    class Window final : public WindowBase
    {
      public:
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

        using WindowBase::getConstant;

      private:
        void close(bool allowExceptions);

        StrongRef<GraphicsBase> graphics;
    };
} // namespace love
