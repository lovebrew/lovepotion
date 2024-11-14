#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class JPGHandler : public FormatHandler
    {
      public:
        bool canDecode(Data* data) const override;

        DecodedImage decode(Data* data) const override;
    };
} // namespace love
