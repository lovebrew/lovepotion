#pragma once

#include "common/math.hpp"
#include "modules/image/FormatHandler.hpp"

namespace love
{
    class T3XHandler : public FormatHandler
    {
      public:
        bool canDecode(Data* data) const override;

        DecodedImage decode(Data* data) const override;

        bool canParseCompressed(Data* data) const override;

        StrongRef<ByteData> parseCompressed(Data* filedata, CompressedSlices& images,
                                            PixelFormat& format) const override;
    };
} // namespace love
