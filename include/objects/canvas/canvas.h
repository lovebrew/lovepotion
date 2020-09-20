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

            void Draw(const DrawArgs & args, const Color & color);
            void Draw(const DrawArgs & args, love::Quad * quad, const Color & color);

            void SetAsTarget();

        private:
            bool cleared;
            Renderer * renderer;

            ImageHandle subHandle;

            #if defined(_3DS)
                C3D_Tex citroTex;
            #endif
    };
}
