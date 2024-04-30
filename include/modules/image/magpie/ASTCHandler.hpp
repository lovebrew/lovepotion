#pragma once

#include "modules/image/FormatHandler.hpp"

namespace love
{
    class ASTCHandler : public FormatHandler
    {
      public:
        virtual ~ASTCHandler()
        {}

        bool canParseCompressed(Data* data) const override;

        StrongRef<ByteData> parseCompressed(Data* data, CompressedSlices& images,
                                            PixelFormat& format) const override;
    };
} // namespace love
