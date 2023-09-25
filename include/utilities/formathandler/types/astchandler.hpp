#pragma once

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>
#include <utilities/formathandler/formathandler.hpp>

namespace love
{
    class ASTCHandler : public FormatHandler
    {
      public:
        struct BlockDim
        {
            uint32_t x;
            uint32_t y;

            bool operator==(const BlockDim&) const = default;
        };

        virtual ~ASTCHandler()
        {}

        bool CanParseCompressed(Data* data) override;

        StrongReference<ByteData> ParseCompressed(
            Data* filedata, std::vector<StrongReference<CompressedSlice>>& images,
            PixelFormat& format, bool& sRGB) override;

        // clang-format off
        static constexpr BidirectionalMap blockDims = 
        {
            BlockDim {4,   4},   PIXELFORMAT_ASTC_4x4,
            BlockDim {5,   4},   PIXELFORMAT_ASTC_5x4,
            BlockDim {5,   5},   PIXELFORMAT_ASTC_5x5,
            BlockDim {6,   5},   PIXELFORMAT_ASTC_6x5,
            BlockDim {6,   6},   PIXELFORMAT_ASTC_6x6,
            BlockDim {8,   5},   PIXELFORMAT_ASTC_8x5,
            BlockDim {8,   6},   PIXELFORMAT_ASTC_8x6,
            BlockDim {8,   8},   PIXELFORMAT_ASTC_8x8,
            BlockDim {10,  5},   PIXELFORMAT_ASTC_10x5,
            BlockDim {10,  6},   PIXELFORMAT_ASTC_10x6,
            BlockDim {10,  8},   PIXELFORMAT_ASTC_10x8,
            BlockDim {10, 10},   PIXELFORMAT_ASTC_10x10,
            BlockDim {12, 10},   PIXELFORMAT_ASTC_12x10,
            BlockDim {12, 12},   PIXELFORMAT_ASTC_12x12
        };
        // clang-format on

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