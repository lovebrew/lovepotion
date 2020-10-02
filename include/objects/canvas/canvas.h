#pragma once

#include "objects/texture/texture.h"

namespace love
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
            ~Canvas();

            void SetAsTarget();
            void Clear(const Color & color);

            Renderer * GetRenderer() {
                return this->renderer;
            }

            TextureHandle GetTextureHandle() {
                return this->texture;
            }

            bool HasFirstClear() {
                return this->cleared;
            }

        private:
            bool cleared;
            Renderer * renderer;

            #if defined(_3DS)
                C3D_Tex citroTex;
            #endif
    };
}
