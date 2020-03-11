#pragma once

#include "common/mmath.h"
#include "objects/texture/texture.h"

namespace love
{
    class Image : public Texture
    {
        public:
            static love::Type type;

            Image(const std::string & path);
            ~Image();

            void Draw(const DrawArgs & args, const Color & color);
            void Draw(const DrawArgs & args, Quad * quad, const Color & color);

        private:
            TextureType textureType;
            TextureHandle texture;

    };
}
