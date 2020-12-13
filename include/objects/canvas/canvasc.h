#pragma once

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
                    int width = 1;
                    int height = 1;
                };

                Canvas(const Settings & settings);

                virtual void Draw(Graphics * gfx, love::Quad * quad, const Matrix4 & localTransform) = 0;

                bool HasFirstClear() {
                    return this->cleared;
                }

            protected:
                Settings settings;
                bool cleared;
        };
    }
}
