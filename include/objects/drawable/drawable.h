#pragma once

/*
** drawable.h
** Superclass for Textures
*/

#include "common/mmath.h"

namespace love
{
    class Drawable : public Object
    {
        public:
            static love::Type type;

            virtual ~Drawable() {};

            virtual void Draw(const DrawArgs & args, const Color & color) = 0;
    };
}
