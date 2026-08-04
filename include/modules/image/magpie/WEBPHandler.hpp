#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class WEBPHandler : public FormatHandler
    {
      public:
        virtual ~WEBPHandler() override
        {}

        bool canDecode(Data* data) const override;

        DecodedImage decode(Data* data) const override;

        void freeRawPixels(unsigned char* memory) override;
    };
} // namespace love
