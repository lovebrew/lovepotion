#pragma once

#include "objects/imagedata/types/formathandler.h"

namespace love
{
    class JPGHandler : public FormatHandler
    {
      public:
        virtual bool CanDecode(Data* data);

        virtual DecodedImage Decode(Data* data);

        virtual void FreeRawPixels(unsigned char* memory);

        virtual const char* GetName()
        {
            return "JPGHandler";
        }
    };
} // namespace love
