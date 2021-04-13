#pragma once

/*
** drawable.h
** Superclass for Textures
*/

#include "common/matrix.h"
#include "objects/object.h"

namespace love
{
    class Graphics;

    class Drawable : public Object
    {
      public:
        static love::Type type;

        virtual ~Drawable() {};

        virtual void Draw(Graphics* gfx, const Matrix4& localTransform) = 0;
    };
} // namespace love
