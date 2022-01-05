#include "objects/compressedimagedata/handlers/astchandler.h"
#include "common/exception.h"

using namespace love;

static PixelFormat convertFormat(uint32_t blockX, uint32_t blockY, uint32_t blockZ)
{
    if (blockZ > 1)
        return PIXELFORMAT_UNKNOWN;

    if (blockX == 4 && blockY == 4)
        return PIXELFORMAT_ASTC_4x4;
    else if (blockX == 5 && blockY == 4)
        return PIXELFORMAT_ASTC_5x4;
    else if (blockX == 5 && blockY == 5)
        return PIXELFORMAT_ASTC_5x5;
    else if (blockX == 6 && blockY == 5)
        return PIXELFORMAT_ASTC_6x5;
    else if (blockX == 6 && blockY == 6)
        return PIXELFORMAT_ASTC_6x6;
    else if (blockX == 8 && blockY == 5)
        return PIXELFORMAT_ASTC_8x5;
    else if (blockX == 8 && blockY == 6)
        return PIXELFORMAT_ASTC_8x6;
    else if (blockX == 8 && blockY == 8)
        return PIXELFORMAT_ASTC_8x8;
    else if (blockX == 10 && blockY == 5)
        return PIXELFORMAT_ASTC_10x5;
    else if (blockX == 10 && blockY == 6)
        return PIXELFORMAT_ASTC_10x6;
    else if (blockX == 10 && blockY == 8)
        return PIXELFORMAT_ASTC_10x8;
    else if (blockX == 10 && blockY == 10)
        return PIXELFORMAT_ASTC_10x10;
    else if (blockX == 12 && blockY == 10)
        return PIXELFORMAT_ASTC_12x10;
    else if (blockX == 12 && blockY == 12)
        return PIXELFORMAT_ASTC_12x12;

    return PIXELFORMAT_UNKNOWN;
}

bool ASTCHandler::CanParseCompressed(Data* data)
{
    if (data->GetSize() <= sizeof(ASTCHeader))
        return false;

    const ASTCHeader* header = (const ASTCHeader*)data->GetData();

    uint32_t identifier = (uint32_t)header->identifier[0] + ((uint32_t)header->identifier[1] << 8) +
                          ((uint32_t)header->identifier[2] << 16) +
                          ((uint32_t)header->identifier[3] << 24);

    if (identifier != ASTC_IDENTIFIER)
        return false;

    return true;
}

StrongReference<CompressedMemory> ASTCHandler::ParseCompressed(
    Data* fileData, std::vector<StrongReference<CompressedSlice>>& images, PixelFormat& format,
    bool& sRGB)
{
    if (!this->CanParseCompressed(fileData))
        throw love::Exception("Could not decode compressed data (not a *.astc file?)");

    ASTCHeader header = *(const ASTCHeader*)fileData->GetData();

    PixelFormat pixelFormat = convertFormat(header.blockdimX, header.blockdimY, header.blockdimZ);

    if (pixelFormat == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Could not parse *.astc file: unsupported ASTC format %dx%dx%d.",
                              header.blockdimX, header.blockdimY, header.blockdimZ);

    uint32_t sizeX = header.sizeX[0] + (header.sizeX[1] << 8) + (header.sizeX[2] << 16);
    uint32_t sizeY = header.sizeY[0] + (header.sizeY[1] << 8) + (header.sizeY[2] << 16);
    uint32_t sizeZ = header.sizeZ[0] + (header.sizeZ[1] << 8) + (header.sizeZ[2] << 16);

    uint32_t blocksX = (sizeX + header.blockdimX - 1) / header.blockdimX;
    uint32_t blocksY = (sizeY + header.blockdimY - 1) / header.blockdimY;
    uint32_t blocksZ = (sizeZ + header.blockdimZ - 1) / header.blockdimZ;

    size_t totalsize = blocksX * blocksY * blocksZ * 16;

    if (totalsize + sizeof(header) > fileData->GetSize())
        throw love::Exception("Could not parse *.astc file: file is too small.");

    StrongReference<CompressedMemory> memory(new CompressedMemory(totalsize), Acquire::NORETAIN);

    // .astc files only store a single mipmap level.
    memcpy(memory->data, (uint8_t*)fileData->GetData() + sizeof(ASTCHeader), totalsize);

    images.emplace_back(new CompressedSlice(pixelFormat, sizeX, sizeY, memory, 0, totalsize),
                        Acquire::NORETAIN);

    format = pixelFormat;
    sRGB   = false;

    return memory;
}
