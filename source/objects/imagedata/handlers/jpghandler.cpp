#include "objects/imagedata/handlers/jpghandler.h"

#include "common/exception.h"
#include <turbojpeg.h>

using namespace love;

bool JPGHandler::CanDecode(Data* data)
{
    tjhandle handle = tjInitDecompress();

    if (handle == NULL)
        return false;

    int samples;

    int width  = 0;
    int height = 0;

    if (tjDecompressHeader2(handle, (uint8_t*)data->GetData(), data->GetSize(), &width, &height,
                            &samples) == -1)
    {
        tjDestroy(handle);
        return false;
    }

    tjDestroy(handle);

    return true;
}

JPGHandler::DecodedImage JPGHandler::Decode(Data* data)
{
    tjhandle handle = tjInitDecompress();
    DecodedImage decoded {};

    if (handle == NULL)
        throw love::Exception("Failed to initialize JPG decompressor.");

    int samples;

    int width  = 0;
    int height = 0;

    if (tjDecompressHeader2(handle, (uint8_t*)data->GetData(), data->GetSize(), &width, &height,
                            &samples) == -1)
    {
        tjDestroy(handle);
        throw love::Exception("Failed to read JPG header.");
    }

    decoded.width  = width;
    decoded.height = height;
    decoded.format = PIXELFORMAT_RGBA8;
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

void JPGHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
