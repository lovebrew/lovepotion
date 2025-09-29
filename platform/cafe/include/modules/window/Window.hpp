#pragma once

#include "common/StrongRef.hpp"

#include "modules/window/Window.tcc"

#include "modules/graphics/Graphics.hpp"

namespace love
{
    class Window final : public WindowBase
    {
      public:
        Window();

        virtual ~Window();

        void close() override;

        void setGraphics(GraphicsBase* graphics)
        {
            this->graphics.set(graphics);
        }

        void updateSettings(const WindowSettings& settings, bool updateGraphicsViewport) override;

        bool setWindow(int width = 800, int height = 600, WindowSettings* settings = nullptr);

        bool onSizeChanged(int width, int height);

        void setDisplaySleepEnabled(bool enable) override;

        bool isDisplaySleepEnabled() const override;

        using WindowBase::getConstant;

      private:
        void close(bool allowExceptions);

        StrongRef<GraphicsBase> graphics;
    };
} // namespace love
