#pragma once

#include "objects/imagedata/types/formathandler.h"

namespace love
{
    class T3XHandler : public FormatHandler
    {
      public:
#if not defined(__3DS__)
        using FormatHandler::CanDecode;
        using FormatHandler::Decode;
#else
        virtual bool CanDecode(Data* data);

        virtual DecodedImage Decode(Data* data);
#endif
        virtual void FreeRawPixels(unsigned char* memory);

      private:
        struct __attribute__((packed)) Tex3DSHeader
        {
            u16 numSubTextures; //< 1
            u8 width_log2 : 3;  //< log2(texWidth)  - 3 (powTwoWidth)
            u8 height_log2 : 3; //< log2(texHeight) - 3 (powTwoHeight)
            u8 type : 1;        //< 0 = GPU_TEX_2D
            u8 format;          //< 0 = GPU_RGBA8
            u8 mipmapLevels;    //< 0

            u16 width;    //< subtexWidth  (sourceWidth)
            u16 height;   //< subtexHeight (sourceHeight)
            float left;   //< left   = border
            float top;    //< top    = texHeight - border
            float right;  //< right  = subtexWidth + border
            float bottom; //< bottom = texHeight - border - subtexHeight

            u32 size : 24;           //< texture size in bytes; should be 4 * texWidth * texHeight
            u32 compressionType : 8; //< 0 (no compression)
        };
    };
} // namespace love
