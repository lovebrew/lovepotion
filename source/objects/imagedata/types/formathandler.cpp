#include "objects/imagedata/types/formathandler.h"
#include "common/exception.h"

#include <vector>

using namespace love;

FormatHandler::FormatHandler()
{}

FormatHandler::~FormatHandler()
{}

bool FormatHandler::CanDecode(Data* /*data*/)
{
    return false;
}

bool FormatHandler::CanEncode(PixelFormat /*rawFormat*/, EncodedFormat /*encodedFormat*/)
{
    return false;
}

FormatHandler::DecodedImage FormatHandler::Decode(Data* /*data*/)
{
    throw love::Exception("Image decoding is not implemented for this format backend.");
}

FormatHandler::EncodedImage FormatHandler::Encode(const DecodedImage& /*img*/,
                                                  EncodedFormat /*format*/)
{
    throw love::Exception("Image encoding is not implemented for this format backend.");
}

bool FormatHandler::CanParseCompressed(Data* /*data*/)
{
    return false;
}

StrongReference<CompressedMemory> FormatHandler::ParseCompressed(
    Data* /*filedata*/, std::vector<StrongReference<CompressedSlice>>& /*images*/,
    PixelFormat& /*format*/, bool& /*sRGB*/)
{
    throw love::Exception("Compressed image parsing is not implemented for this format backend.");
}

void FormatHandler::FreeRawPixels(unsigned char* memory)
{
    delete[] memory;
}
