#pragma once

#include "common/lmath.h"
#include "modules/filesystem/wrap_filesystem.h"
#include "objects/texture/texture.h"

namespace love
{
    class Image : public Texture
    {
      public:
        static love::Type type;

        Image(Data* data);

        ~Image();

        Image(TextureType type, int width, int height);

        void Init(int width, int height);

#if defined(__SWITCH__)
        void ReplacePixels(const void* data, size_t size, const Rect& rect);
#endif

      private:
        TextureType textureType;
    };
} // namespace love
