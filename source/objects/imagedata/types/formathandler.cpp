#include "objects/imagedata/types/formathandler.h"
#include "common/exception.h"

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

bool FormatHandler::CanParseCompressed(Data* /*data*/)
{
    return false;
}

void FormatHandler::FreeRawPixels(unsigned char* memory)
{
    delete[] memory;
}
