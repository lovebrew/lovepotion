#include "objects/imagedata/handlers/jpghandler.h"

#include "common/exception.h"
#include <turbojpeg.h>

using namespace love;

bool JPGHandler::CanDecode(Data* data)
{
    tjhandle _jpegDecompressor = tjInitDecompress();

    if (_jpegDecompressor == NULL)
        return false;

    int samples;

    int width  = 0;
    int height = 0;

    if (tjDecompressHeader2(_jpegDecompressor, (u8*)data->GetData(), data->GetSize(), &width,
                            &height, &samples) == -1)
    {
        tjDestroy(_jpegDecompressor);
        return false;
    }

    tjDestroy(_jpegDecompressor);

    return true;
}

bool JPGHandler::CanEncode(PixelFormat rawFormat, EncodedFormat encodedFormat)
{
    return false;
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

    if (tjDecompressHeader2(handle, (u8*)data->GetData(), data->GetSize(), &width, &height,
                            &samples) == -1)
    {
        tjDestroy(handle);
        throw love::Exception("Failed to read JPG header.");
    }

    decoded.width  = width;
    decoded.height = height;
    decoded.format = PIXELFORMAT_RGBA8;

    decoded.data = new uint8_t[width * height * 4];

    /* we always want RGBA, hopefully outputs as RGBA8 */
    if (tjDecompress2(handle, (u8*)data->GetData(), data->GetSize(), decoded.data, width, 0, height,
                      TJPF_RGBA, TJFLAG_ACCURATEDCT) == -1)
    {
        tjDestroy(handle);
        throw love::Exception("Could not decode JPG image (%s)", tjGetErrorStr());
    }

    tjDestroy(handle);

    return decoded;
}

JPGHandler::EncodedImage JPGHandler::Encode(const DecodedImage& decoded, EncodedFormat format)
{
    tjhandle handle = tjInitCompress();
    EncodedImage encoded {};

    if (handle == NULL)
        throw love::Exception("Failed to initialize JPG compressor.");

    encoded.data = new uint8_t[decoded.size];
    encoded.size = decoded.size;

    if (tjCompress2(handle, decoded.data, decoded.width, 0, decoded.height, TJPF_RGBA,
                    &encoded.data, (unsigned long*)&decoded.size, TJSAMP_444, 100,
                    TJFLAG_ACCURATEDCT) == -1)
    {
        tjDestroy(handle);
        throw love::Exception("Could not encode JPG image (%s)", tjGetErrorStr());
    };

    tjDestroy(handle);

    return encoded;
}

void JPGHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
