#pragma once
#include <modules/graphics_ext.hpp>

namespace love
{
    class TempTransform
    {
      public:
        TempTransform(Graphics<Console::Which>& graphics) : graphics(&graphics)
        {
            this->graphics->PushTransform();
        }

        TempTransform(Graphics<Console::Which>& graphics, const Matrix4& transform) :
            TempTransform(graphics)
        {
            this->graphics->InternalScale(transform);
        }

        template<typename vDst, typename vSrc>
        void TransformXY(vDst&& dst, vSrc&& src)
        {
            this->graphics->GetTransform().TransformXY(std::forward<decltype(dst)>(dst),
                                                       std::forward<decltype(src)>(src));
        }

        template<typename vDst, typename vSrc>
        void TransformXYPure(vDst&& dst, vSrc&& src)
        {
            this->graphics->GetTransform().TransformXY(std::forward<decltype(dst)>(dst),
                                                       std::forward<decltype(src)>(src));
        }

        ~TempTransform()
        {
            printf("[TempTransform::~TempTransform] Popping tempTransform\n");
            this->graphics->PopTransform();
        }

      private:
        Graphics<Console::Which>* graphics;
    };
} // namespace love
