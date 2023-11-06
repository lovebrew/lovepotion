#pragma once

#include "matrix.tcc"
#include "object.hpp"

namespace love
{
    template<Console::Platform T>
    class Graphics;

    class Drawable : public Object
    {
      public:
        static inline Type type = Type("Drawable", &Object::type);

        virtual ~Drawable()
        {}

        virtual void Draw(Graphics<Console::Which>& graphics,
                          const Matrix4& matrix) = 0;
    };
} // namespace love
