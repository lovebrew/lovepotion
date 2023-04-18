#pragma once

#include <modules/graphics_ext.hpp>

namespace love
{
    class TempTransform
    {
      public:
        TempTransform(Graphics<Console::Which>& graphics) : graphics(graphics)
        {
            this->graphics.PushTransform();
        }

        TempTransform(Graphics<Console::Which>& graphics, const Matrix4<Console::Which>& transform)
        {
            this->graphics.PushTransform();
            this->graphics.InternalScale(transform);
        }

        template<typename vDst, typename vSrc>
        void TransformXY(vDst dst, vSrc src, int count)
        {
            this->graphics.GetTransform().TransformXYVert(dst, src, count);
        }

        ~TempTransform()
        {
            this->graphics.PopTransform();
        }

      private:
        Graphics<Console::Which> graphics;
    };
} // namespace love
