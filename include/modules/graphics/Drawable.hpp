#pragma once

#include "common/Matrix.hpp"
#include "common/Object.hpp"

namespace love
{
    class GraphicsBase;

    class Drawable : public Object
    {
      public:
        static inline Type type = Type("Drawable", &Object::type);

        virtual ~Drawable()
        {}

        virtual void draw(GraphicsBase* graphics, const Matrix4& transform) = 0;
    };
} // namespace love
