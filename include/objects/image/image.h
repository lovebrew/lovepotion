#pragma once

#include "common/lmath.h"
#include "modules/filesystem/wrap_filesystem.h"
#include "objects/texture/texture.h"

#include "objects/compressedimagedata/compressedimagedata.h"
#include "objects/imagedata/imagedata.h"

namespace love
{
    class Image : public Texture
    {
      public:
        static love::Type type;

        enum MipmapsType
        {
            MIPMAPS_NONE,
            MIPMAPS_DATA,
            MIPMAPS_GENERATED,
        };

        struct Slices
        {
          public:
            Slices(TextureType type);

            void Clear();

            void Set(int slice, int mipmap, ImageDataBase* data);

            ImageDataBase* Get(int slice, int mipmap) const;

            void Add(CompressedImageData* data, int start, int startmip, bool addAllSlices,
                     bool addAllMips);

            int GetSliceCount(int mip = 0) const;

            int GetMipmapCount(int slice = 0) const;

            MipmapsType Validate() const;

            TextureType GetTextureType() const
            {
                return this->textureType;
            }

          private:
            TextureType textureType;
            std::vector<std::vector<StrongReference<ImageDataBase>>> data;
        };

#if defined(__SWITCH__)
        void ReplacePixels(const void* data, size_t size, const Rect& rect);
#endif

        Image(const Slices& data);

        ~Image();

        Image(TextureType type, int width, int height);

        void Init(int width, int height);

      private:
        TextureType textureType;
    };
} // namespace love
