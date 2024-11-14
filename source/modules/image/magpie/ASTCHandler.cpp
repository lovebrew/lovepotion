#include "common/Exception.hpp"

#include "modules/image/magpie/ASTCHandler.hpp"

#include <cstring>

#define E_COULD_NOT_PARSE "Could not parse .astc file: unsupported ASTC format {:d}x{:d}x{:d}."

namespace love
{
    static constexpr uint32_t ASTC_MAGIC = 0x5CA1AB13;

#pragma pack(push, 1)
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
#pragma pack(pop)

    static PixelFormat convertFormat(uint32_t blockX, uint32_t blockY, uint32_t blockZ)
    {
        if (blockZ > 1)
            return PIXELFORMAT_UNKNOWN;

        if (blockX == 4 && blockY == 4)
            return PIXELFORMAT_ASTC_4x4_UNORM;
        else if (blockX == 5 && blockY == 4)
            return PIXELFORMAT_ASTC_5x4_UNORM;
        else if (blockX == 5 && blockY == 5)
            return PIXELFORMAT_ASTC_5x5_UNORM;
        else if (blockX == 6 && blockY == 5)
            return PIXELFORMAT_ASTC_6x5_UNORM;
        else if (blockX == 6 && blockY == 6)
            return PIXELFORMAT_ASTC_6x6_UNORM;
        else if (blockX == 8 && blockY == 5)
            return PIXELFORMAT_ASTC_8x5_UNORM;
        else if (blockX == 8 && blockY == 6)
            return PIXELFORMAT_ASTC_8x6_UNORM;
        else if (blockX == 8 && blockY == 8)
            return PIXELFORMAT_ASTC_8x8_UNORM;
        else if (blockX == 10 && blockY == 5)
            return PIXELFORMAT_ASTC_10x5_UNORM;
        else if (blockX == 10 && blockY == 6)
            return PIXELFORMAT_ASTC_10x6_UNORM;
        else if (blockX == 10 && blockY == 8)
            return PIXELFORMAT_ASTC_10x8_UNORM;
        else if (blockX == 10 && blockY == 10)
            return PIXELFORMAT_ASTC_10x10_UNORM;
        else if (blockX == 12 && blockY == 10)
            return PIXELFORMAT_ASTC_12x10_UNORM;
        else if (blockX == 12 && blockY == 12)
            return PIXELFORMAT_ASTC_12x12_UNORM;

        return PIXELFORMAT_UNKNOWN;
    }

    bool ASTCHandler::canParseCompressed(Data* data) const
    {
        if (data->getSize() <= sizeof(ASTCHeader))
            return false;

        const auto* header = (const ASTCHeader*)data->getData();

        const auto header0 = (uint32_t)header->identifier[0];
        const auto header1 = (uint32_t)header->identifier[1] << 8;
        const auto header2 = (uint32_t)header->identifier[2] << 16;
        const auto header3 = (uint32_t)header->identifier[3] << 24;

        return (header0 + header1 + header2 + header3) == ASTC_MAGIC;
    }

    StrongRef<ByteData> ASTCHandler::parseCompressed(Data* filedata, CompressedSlices& images,
                                                     PixelFormat& format) const
    {
        if (!this->canParseCompressed(filedata))
            throw love::Exception("Could not decode compressed data (not an .astc file?)");

        ASTCHeader header    = *(const ASTCHeader*)filedata->getData();
        auto convertedFormat = convertFormat(header.blockdimX, header.blockdimY, header.blockdimZ);

        if (convertedFormat == PIXELFORMAT_UNKNOWN)
            throw love::Exception(E_COULD_NOT_PARSE, header.blockdimX, header.blockdimY, header.blockdimZ);

        const auto sizeX = header.sizeX[0] + (header.sizeX[1] << 8) + (header.sizeX[2] << 16);
        const auto sizeY = header.sizeY[0] + (header.sizeY[1] << 8) + (header.sizeY[2] << 16);
        const auto sizeZ = header.sizeZ[0] + (header.sizeZ[1] << 8) + (header.sizeZ[2] << 16);

        const auto blocksX = (sizeX + header.blockdimX - 1) / header.blockdimX;
        const auto blocksY = (sizeY + header.blockdimY - 1) / header.blockdimY;
        const auto blocksZ = (sizeZ + header.blockdimZ - 1) / header.blockdimZ;

        const auto totalSize = (size_t)blocksX * blocksY * blocksZ * 16;

        if (totalSize + sizeof(ASTCHeader) > filedata->getSize())
            throw love::Exception("Could not parse .astc file: file is too small.");

        StrongRef<ByteData> memory(new ByteData(totalSize, false), Acquire::NO_RETAIN);
        std::memcpy(memory->getData(), (uint8_t*)filedata->getData() + sizeof(ASTCHeader), totalSize);

        // clang-format off
        images.emplace_back(new CompressedSlice(convertedFormat, sizeX, sizeY, memory, 0, totalSize), Acquire::NO_RETAIN);
        // clang-format on

        format = convertedFormat;
        return memory;
    }
} // namespace love
