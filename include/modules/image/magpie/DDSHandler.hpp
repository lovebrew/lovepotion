#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class DDSHandler : public FormatHandler
    {
      public:
        virtual ~DDSHandler()
        {}

        bool canDecode(Data* data) const override;

        DecodedImage decode(Data* data) const override;

        bool canParseCompressed(Data* data) const override;

        StrongRef<ByteData> parseCompressed(Data* data, CompressedSlices& images,
                                            PixelFormat& format) const override;
    };
} // namespace love
