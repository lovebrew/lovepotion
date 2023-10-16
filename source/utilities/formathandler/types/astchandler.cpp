#include <common/exception.hpp>

#include <objects/compressedimagedata/compressedslice.hpp>
#include <utilities/formathandler/types/astchandler.hpp>

using namespace love;
using CompressedSlices = std::vector<StrongReference<CompressedSlice>>;

static PixelFormat convertFormat(uint32_t blockX, uint32_t blockY, uint32_t blockZ)
{
    if (blockZ > 1)
        return PIXELFORMAT_UNKNOWN;

    std::optional<PixelFormat> format;
    if (!(format = ASTCHandler::blockDims.Find({ blockX, blockY })))
        return PIXELFORMAT_UNKNOWN;

    return *format;
}

bool ASTCHandler::CanParseCompressed(Data* data)
{
    if (data->GetSize() <= sizeof(ASTCHeader))
        return false;

    const auto* header = (const ASTCHeader*)data->GetData();

    // clang-format off
    const auto identifier =   (uint32_t)header->identifier[0]
                            + ((uint32_t)header->identifier[1] << 8)
                            + ((uint32_t)header->identifier[2] << 16)
                            + ((uint32_t)header->identifier[3] << 24);
    // clang-format on

    if (identifier != ASTC_IDENTIFIER)
        return false;

    return true;
}

StrongReference<ByteData> ASTCHandler::ParseCompressed(Data* data, CompressedSlices& images,
                                                       PixelFormat& format, bool& srgb)
{
    if (!this->CanParseCompressed(data))
        throw love::Exception("Could not decode compressed data (not an .astc file?)");

    const auto header   = *(const ASTCHeader*)data->GetData();
    PixelFormat _format = convertFormat(header.blockdimX, header.blockdimY, header.blockdimZ);

    if (_format == PIXELFORMAT_UNKNOWN)
    {
        throw love::Exception("Could not parse .astc file: unsupported ASTC format %dx%dx%d.",
                              header.blockdimX, header.blockdimY, header.blockdimZ);
    }

    uint32_t sizeX = header.sizeX[0] + (header.sizeX[1] << 8) + (header.sizeX[2] << 16);
    uint32_t sizeY = header.sizeY[0] + (header.sizeY[1] << 8) + (header.sizeY[2] << 16);
    uint32_t sizeZ = header.sizeZ[0] + (header.sizeZ[1] << 8) + (header.sizeZ[2] << 16);

    uint32_t blocksX = (sizeX + header.blockdimX - 1) / header.blockdimX;
    uint32_t blocksY = (sizeY + header.blockdimY - 1) / header.blockdimY;
    uint32_t blocksZ = (sizeZ + header.blockdimZ - 1) / header.blockdimZ;

    size_t totalSize = (size_t)blocksX * blocksY * blocksZ * 16;

    if (totalSize + sizeof(header) > data->GetSize())
        throw love::Exception("Could not parse .astc file: file is too small.");

    StrongReference<ByteData> memory(new ByteData(totalSize, false), Acquire::NORETAIN);
    std::memcpy(memory->GetData(), (uint8_t*)data->GetData() + sizeof(ASTCHeader), totalSize);

    images.emplace_back(new CompressedSlice(_format, sizeX, sizeY, memory, 0, totalSize),
                        Acquire::NORETAIN);

    format = _format;
    srgb   = false;

    return memory;
}
