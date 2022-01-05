#pragma once

#include "objects/imagedata/types/formathandler.h"

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

        StrongReference<CompressedMemory> ParseCompressed(
            Data* fileData, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& isSRGB) override;

        const char* GetName() override
        {
            return "DDSHandler";
        }
    };
} // namespace love
