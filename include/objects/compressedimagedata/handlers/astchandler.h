#pragma once

#include "objects/imagedata/types/formathandler.h"

namespace love
{
    class ASTCHandler : public FormatHandler
    {
      public:
        virtual ~ASTCHandler()
        {}

        bool CanParseCompressed(Data* data) override;

        StrongReference<CompressedMemory> ParseCompressed(
            Data* fileData, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& sRGB) override;

        const char* GetName() override
        {
            return "ASTCHandler";
        }

      private:
        static constexpr uint32_t ASTC_IDENTIFIER = 0x5CA1AB13;

        struct ASTCHeader
        {
            uint8_t identifier[4];
            uint8_t blockdimX;
            uint8_t blockdimY;
            uint8_t blockdimZ;
            uint8_t sizeX[3];
            uint8_t sizeY[3];
            uint8_t sizeZ[3];
        };
    };
} // namespace love
