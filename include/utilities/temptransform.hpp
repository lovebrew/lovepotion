#pragma once

#include <modules/graphics_ext.hpp>

namespace love
{
    class TempTransform
    {
      public:
        TempTransform(Graphics<Console::Which>* graphics) : graphics(graphics)
        {
            this->graphics->PushTransform();
        }

        TempTransform(Graphics<Console::Which>* graphics, const Matrix4<Console::Which>& transform)
        {
            this->graphics->PushTransform();
            this->graphics->InternalScale(transform);
        }

        ~TempTransform()
        {
            this->graphics->PopTransform();
        }

      private:
        Graphics<Console::Which>* graphics;
    };
} // namespace love
