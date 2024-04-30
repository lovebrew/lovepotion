#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class PKMHandler : public FormatHandler
    {
      public:
        virtual ~PKMHandler()
        {}

        bool canParseCompressed(Data* data) const override;

        StrongRef<ByteData> parseCompressed(Data* data, CompressedSlices& images,
                                            PixelFormat& format) const override;
    };
}
