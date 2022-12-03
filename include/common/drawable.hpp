#pragma once

#include "matrix.tcc"
#include "object.hpp"

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Graphics;

    class Drawable : public Object
    {
      public:
        static inline Type type = Type("Drawable", &Object::type);

        virtual ~Drawable()
        {}

        virtual void Draw(Graphics<Console::Which>* graphics,
                          const Matrix4<Console::Which>& matrix) = 0;
    };
} // namespace love
