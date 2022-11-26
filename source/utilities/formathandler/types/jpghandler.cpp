#include <utilities/formathandler/types/jpghandler.hpp>

#include <common/exception.hpp>

#include <turbojpeg.h>

using namespace love;

bool JPGHandler::CanDecode(Data* data)
{
    auto handle = tjInitDecompress();

    if (!handle)
        return false;

    int samples = 0;
    int width   = 0;
    int height  = 0;

    if (tjDecompressHeader2(handle, (uint8_t*)data->GetData(), data->GetSize(), &width, &height,
                            &samples) < 0)
    {
        tjDestroy(handle);
        return false;
    }

    tjDestroy(handle);

    return true;
}

JPGHandler::DecodedImage JPGHandler::Decode(Data* data)
{
    auto handle = tjInitDecompress();
    DecodedImage decoded {};

    if (handle == NULL)
        throw love::Exception("Failed to initialize JPG decompressor.");

    int samples;

    int width  = 0;
    int height = 0;

    if (tjDecompressHeader2(handle, (uint8_t*)data->GetData(), data->GetSize(), &width, &height,
                            &samples) < 0)
    {
        tjDestroy(handle);
        throw love::Exception("Failed to read JPG header.");
    }

    decoded.width  = width;
    decoded.height = height;
    decoded.format = PIXELFORMAT_RGBA8_UNORM;
    decoded.size   = width * height * 4;

    decoded.data = new (std::align_val_t(4)) uint8_t[decoded.size];

    if (tjDecompress2(handle, (uint8_t*)data->GetData(), data->GetSize(), decoded.data, width, 0,
                      height, TJPF_RGBA, TJFLAG_ACCURATEDCT) == -1)
    {
        tjDestroy(handle);
        throw love::Exception("Could not decode JPG image (%s)", tjGetErrorStr());
    }

    tjDestroy(handle);

    return decoded;
}
