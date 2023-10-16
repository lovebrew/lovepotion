#pragma once

#include <utilities/formathandler/formathandler.hpp>

namespace love
{
    class JPGHandler : public FormatHandler
    {
      public:
        virtual bool CanDecode(Data* data);

        virtual DecodedImage Decode(Data* data);
    };
} // namespace love
