#include <utilities/formathandler/types/t3xhandler.hpp>

#include <common/exception.hpp>

#include <utilities/driver/renderer_ext.hpp>

#include <algorithm>

#include <3ds.h>

using namespace love;

bool T3XHandler::CanDecode(Data* data)
{
    Tex3DSHeader header {};
    std::memcpy(&header, data->GetData(), sizeof(header));

    if (header.numSubTextures != 1)
        return false;
    else if (header.type != GPU_TEX_2D)
        return false;

    return true;
}

T3XHandler::DecodedImage T3XHandler::Decode(Data* data)
{
    Tex3DSHeader header {};
    std::memcpy(&header, data->GetData(), sizeof(header));

    if (header.numSubTextures != 1)
        throw love::Exception("t3x does not contain exactly one subtexture");

    if (header.type != GPU_TEX_2D)
        throw love::Exception("t3x texture type is not GPU_TEX_2D");

    std::optional<PixelFormat> pixelFormat;
    const auto format = (GPU_TEXCOLOR)header.format;

    if (!(pixelFormat = Renderer<Console::CTR>::pixelFormats.ReverseFind(format)))
        throw love::Exception("PixelFormat %u is not compatible", header.format);

    const auto width  = 1 << (header.width_log2 + 3);
    const auto height = 1 << (header.height_log2 + 3);

    if (width > 1024 || height > 1024)
        throw love::Exception("t3x dimensions too large");

    DecodedImage decoded {};

    decoded.width  = header.width;
    decoded.height = header.height;
    decoded.format = *pixelFormat;

    const auto size       = data->GetSize() - sizeof(header);
    const auto compressed = (uint8_t*)(data->GetData() + sizeof(header));

    size_t outSize = 0;
    if (decompressHeader(nullptr, &outSize, nullptr, compressed, size) < 0)
        throw love::Exception("Failed to decompress t3x header!");

    decoded.size = (header.width * header.height * love::GetPixelFormatBlockSize(*pixelFormat));
    decoded.data = std::make_unique<uint8_t[]>(outSize);

    if (!decompress(decoded.data.get(), outSize, nullptr, compressed, size))
        throw love::Exception("Failed to decompress t3x data!");

    return decoded;
}
