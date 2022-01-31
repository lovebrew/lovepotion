#include "objects/imagedata/handlers/jpghandler.h"

#include "common/exception.h"
#include <jpeglib.h>

using namespace love;

bool JPGHandler::CanDecode(Data* data)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(&cinfo, (uint8_t*)data->GetData(), data->GetSize());

    int result = jpeg_read_header(&cinfo, true);
    jpeg_destroy_decompress(&cinfo);

    return result == 1;
}

JPGHandler::DecodedImage JPGHandler::Decode(Data* data)
{
    DecodedImage decoded {};

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    jpeg_mem_src(&cinfo, (uint8_t*)data->GetData(), data->GetSize());

    if (jpeg_read_header(&cinfo, true) != 1)
    {
        jpeg_destroy_decompress(&cinfo);
        throw love::Exception("Could not decode non-JPG image.");
    }

    cinfo.out_color_space = JCS_EXT_RGBA;

    jpeg_start_decompress(&cinfo);

    decoded.width  = cinfo.output_width;
    decoded.height = cinfo.output_height;
    decoded.format = PIXELFORMAT_RGBA8;
    decoded.size   = decoded.width * decoded.height * 4;

    decoded.data = new (std::align_val_t(4)) uint8_t[decoded.size];

    size_t row_stride = decoded.width * 4;

    while (cinfo.output_scanline < cinfo.output_height)
    {
        uint8_t* buffer_array[1];
        buffer_array[0] = decoded.data + (cinfo.output_scanline) * row_stride;

        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    return decoded;
}

void JPGHandler::FreeRawPixels(unsigned char* memory)
{
    if (memory)
        delete[] memory;
}
