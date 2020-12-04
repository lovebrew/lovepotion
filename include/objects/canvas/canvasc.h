#pragma once

#include "objects/texture/texture.h"

namespace love
{
    class Graphics;

    namespace common
    {
        class Canvas : public Texture
        {
            public:
                static love::Type type;

                struct Settings
                {
                    int width = 1;
                    int height = 1;
                };

                Canvas(const Settings & settings);

                virtual void SetAsTarget() = 0;

                virtual void Clear(const Colorf & color) = 0;

                virtual void Draw(Graphics * gfx, Quad * quad, const Matrix4 & localTransform) = 0;

                Renderer * GetRenderer() {
                    return this->renderer;
                }

                TextureHandle & GetTextureHandle() {
                    return this->texture;
                }

                bool HasFirstClear() {
                    return this->cleared;
                }

            protected:
                Settings settings;
                Renderer * renderer;
                bool cleared;
        };
    }
}
