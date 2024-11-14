#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class KTXHandler : public FormatHandler
    {
      public:
        virtual ~KTXHandler()
        {}

        // Implements FormatHandler.
        bool canParseCompressed(Data* data) const override;

        StrongRef<ByteData> parseCompressed(Data* filedata, CompressedSlices& images,
                                            PixelFormat& format) const override;

    }; // KTXHandler
} // namespace love
