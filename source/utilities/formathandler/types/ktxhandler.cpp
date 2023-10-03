#include <common/exception.hpp>

#include <utilities/formathandler/types/ktxhandler.hpp>

#include <objects/compressedimagedata/compressedslice.hpp>

#include <string.h>

#include <bit>
#include <cmath>

using namespace love;
using CompressedSlices = std::vector<StrongReference<CompressedSlice>>;

PixelFormat convertFormat(uint32_t glFormat, bool& sRGB)
{
    sRGB = false;
    std::optional<KTXHandler::ConvertedFormat> format;

    if (!(format = KTXHandler::ktxFormats.Find(glFormat)))
        return PIXELFORMAT_UNKNOWN;

    sRGB = format->sRGB;
    return format->format;
}

bool KTXHandler::CanParseCompressed(Data* data)
{
    if (data->GetSize() < sizeof(KTXHeader))
        return false;

    auto* header     = (KTXHeader*)data->GetData();
    auto* identifier = KTXHandler::IDENTIFIER_REF;

    if (memcmp(header->identifier, identifier, 12) != 0)
        return false;

    if (header->endianness != KTX_ENDIAN_REF && header->endianness != KTX_ENDIAN_REF_REV)
        return false;

    return true;
}

StrongReference<ByteData> KTXHandler::ParseCompressed(Data* data, CompressedSlices& images,
                                                      PixelFormat& format, bool& sRGB)
{
    if (!this->CanParseCompressed(data))
        throw love::Exception("Could not decode compressed data (not a KTX file?)");

    auto header = *(KTXHeader*)data->GetData();
    if (header.endianness == KTX_ENDIAN_REF_REV)
    {
        uint32_t* array = (uint32_t*)&header.glType;
        for (int index = 0; index < 12; index++)
            array[index] = std::byteswap(array[index]);
    }

    header.numberOfMipmapLevels = std::max(header.numberOfMipmapLevels, (uint32_t)1);

    bool isSRGB  = false;
    auto _format = convertFormat(header.glInternalFormat, isSRGB);

    if (_format == PIXELFORMAT_UNKNOWN)
        throw love::Exception("Unsupported image format in KTX file.");

    if (header.numberOfArrayElements > 0)
        throw love::Exception("Texture arrays in KTX files are not supported.");

    if (header.pixelDepth > 1)
        throw love::Exception("3D textures in KTX files are not supported.");

    if (header.numberOfFaces > 1)
        throw love::Exception("Cubemap textures in KTX files are not supported.");

    size_t fileOffset     = sizeof(KTXHeader) + header.bytesOfKeyValueData;
    const auto* fileBytes = (uint8_t*)data->GetData();

    size_t totalSize = 0;

    for (int index = 0; index < (int)header.numberOfMipmapLevels; index++)
    {
        if (fileOffset + sizeof(uint32_t) > data->GetSize())
            throw love::Exception("Could not parse KTX file: unexpected EOF.");

        auto mipSize = *(uint32_t*)(fileBytes + fileOffset);
        if (header.endianness == KTX_ENDIAN_REF_REV)
            mipSize = std::byteswap(mipSize);

        fileOffset += sizeof(uint32_t);
        uint32_t mipSizePadded = (mipSize + 3) & ~uint32_t(3);

        totalSize += mipSizePadded;
        fileOffset += mipSizePadded;
    }

    StrongReference<ByteData> memory(new ByteData(totalSize, false), Acquire::NORETAIN);
    fileOffset        = sizeof(KTXHeader) + header.bytesOfKeyValueData;
    size_t dataOffset = 0;

    for (int index = 0; index < (int)header.numberOfMipmapLevels; index++)
    {
        auto mipSize = *(uint32_t*)(fileBytes + fileOffset);
        if (header.endianness == KTX_ENDIAN_REF_REV)
            mipSize = std::byteswap(mipSize);

        fileOffset += sizeof(uint32_t);
        uint32_t mipSizePadded = (mipSize + 3) & ~uint32_t(3);

        int width  = (int)std::max(header.pixelWidth >> index, (uint32_t)1);
        int height = (int)std::max(header.pixelHeight >> index, (uint32_t)1);

        std::memcpy((uint8_t*)memory->GetData() + dataOffset, fileBytes + fileOffset, mipSize);
        auto slice = new CompressedSlice(_format, width, height, memory, dataOffset, mipSize);

        images.push_back(slice);
        slice->Release();

        fileOffset += mipSizePadded;
        dataOffset += mipSizePadded;
    }

    format = _format;
    sRGB   = isSRGB;

    return memory;
}