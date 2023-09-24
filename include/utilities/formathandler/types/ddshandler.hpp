#pragma once

#include <utilities/formathandler/formathandler.hpp>

namespace love
{
    class DDSHandler : public FormatHandler
    {
      public:
        virtual ~DDSHandler()
        {}

        bool CanDecode(Data* data) override;

        DecodedImage Decode(Data* data) override;

        bool CanParseCompressed(Data* data) override;

        StrongReference<ByteData> ParseCompressed(
            Data* filedata, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& sRGB) override;
    };
} // namespace love