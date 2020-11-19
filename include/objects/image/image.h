#pragma once

#include "common/mmath.h"
#include "objects/texture/texture.h"
#include "modules/filesystem/wrap_filesystem.h"

#if defined (_3DS)
    typedef C2D_SpriteSheet TextureSheet;
#elif defined (__SWITCH__)
    typedef struct _SpriteSheet {} TextureSheet;
#endif

namespace love
{
    class Image : public Texture
    {
        public:
            static love::Type type;

            Image(Data * data);

            Image(TextureType type, int width, int height);

            void LoadBuffer(void * buffer, size_t size);

            void Init(int width, int height);

            #if defined(__SWITCH__)
                void SetTextureHandle(CImage & image);

                void ReplacePixels(void * data, size_t size, const Rect & rect);
            #endif

            ~Image();

        private:
            TextureType textureType;
            TextureSheet sheet;
    };
}
