#pragma once

#include "common/StrongRef.hpp"

#include "modules/window/Window.tcc"

#include "modules/graphics/Graphics.hpp"

namespace love
{
    class Window final : public WindowBase<Window>
    {
      public:
        Window();

        virtual ~Window();

        void setGraphics(Graphics* graphics)
        {
            this->graphics.set(graphics);
        }

        void close();

        bool setWindow(int width = 800, int height = 600, WindowSettings* settings = nullptr);

        void updateSettingsImpl(const WindowSettings& settings, bool updateGraphicsViewport);

        bool onSizeChanged(int width, int height);

        void setDisplaySleepEnabled(bool enable);

        bool isDisplaySleepEnabled() const;

        using WindowBase::getConstant;

      private:
        void close(bool allowExceptions);

        StrongRef<Graphics> graphics;
    };
} // namespace love
