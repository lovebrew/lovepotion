#pragma once

#include <utilities/formathandler/formathandler.hpp>

namespace love
{
    class T3XHandler : public FormatHandler
    {
      public:
        virtual bool CanDecode(Data* data);

        virtual DecodedImage Decode(Data* data);

      private:
        struct __attribute__((packed)) Tex3DSHeader
        {
            uint16_t numSubTextures; //< 1

            uint8_t width_log2 : 3;  //< log2(texWidth)  - 3 (powTwoWidth)
            uint8_t height_log2 : 3; //< log2(texHeight) - 3 (powTwoHeight)

            uint8_t type : 1;     //< 0 = GPU_TEX_2D
            uint8_t format;       //< 0 = GPU_RGBA8
            uint8_t mipmapLevels; //< 0

            uint16_t width;  //< subtexWidth  (sourceWidth)
            uint16_t height; //< subtexHeight (sourceHeight)

            uint16_t left;   //< left   = border
            uint16_t top;    //< top    = texHeight
            uint16_t right;  //< right  = subtexWidth
            uint16_t bottom; //< bottom = texHeight - subtexHeight
        };
    };
} // namespace love
