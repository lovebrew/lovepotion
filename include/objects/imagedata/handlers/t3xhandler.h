#pragma once

#include "objects/imagedata/types/formathandler.h"

namespace love
{
    class T3XHandler : public FormatHandler
    {
      public:
        virtual bool CanDecode(Data* data);

        virtual bool CanEncode(PixelFormat rawFormat, EncodedFormat encodedFormat);

        virtual DecodedImage Decode(Data* data);

        virtual EncodedImage Encode(const DecodedImage& image, EncodedFormat format);

        virtual void FreeRawPixels(unsigned char* memory);

      private:
        struct __attribute__((packed)) Tex3DSHeader
        {
            u16 numSubTextures; // 1
            u8 width_log2 : 3;  // log2(texWidth)  - 3
            u8 height_log2 : 3; // log2(texHeight) - 3
            u8 type : 1;        // 0 = GPU_TEX_2D
            u8 format;          // 0 = GPU_RGBA8
            u8 mipmapLevels;    // 0

            u16 width;    // subtexWidth
            u16 height;   // subtexHeight
            float left;   // left   = border
            float top;    // top    = texHeight - border
            float right;  // right  = subtexWidth + border
            float bottom; // bottom = texHeight - border - subtexHeight

            u32 size : 24;           // texture size in bytes; should be 4 * texWidth * texHeight
            u32 compressionType : 8; // 0=none
        };
    };
} // namespace love
