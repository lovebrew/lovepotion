#pragma once

#if defined(__3DS__)
    #include <citro2d.h>
#elif defined(__SWITCH__)
    #include <deko3d.h>
#endif

#include "objects/texture/texture.h"

namespace love
{
    class Graphics;

    namespace common
    {
        class Canvas : public love::Texture
        {
          public:
            static love::Type type;

            struct Settings
            {
                int width  = 1;
                int height = 1;
            };

            Canvas(const Settings& settings);

            virtual void Draw(Graphics* gfx, love::Quad* quad, const Matrix4& localTransform) = 0;

            bool HasFirstClear()
            {
                return this->cleared;
            }

          protected:
            Settings settings;
            bool cleared;
        };
    } // namespace common
} // namespace love
