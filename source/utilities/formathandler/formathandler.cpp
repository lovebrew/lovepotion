#include <utilities/formathandler/formathandler.hpp>

#include <common/exception.hpp>

#include <objects/compressedimagedata/compressedslice.hpp>

using namespace love;

bool FormatHandler::CanDecode(Data* data)
{
    return false;
}

FormatHandler::DecodedImage FormatHandler::Decode(Data* /* data */)
{
    throw love::Exception("Image decoding is not implemented for this format backend.");
}

bool FormatHandler::CanEncode(PixelFormat /*format*/, EncodedFormat /*encodedFormat*/)
{
    return false;
}

FormatHandler::EncodedImage FormatHandler::Encode(const DecodedImage& /*image*/,
                                                  EncodedFormat /*format*/)
{
    throw love::Exception("Image encoding is not implemented for this format backend.");
}

bool FormatHandler::CanParseCompressed(Data* /*data*/)
{
    return false;
}

StrongReference<ByteData> FormatHandler::ParseCompressed(
    Data* /*filedata*/, std::vector<StrongReference<CompressedSlice>>& /*images*/,
    PixelFormat& /*format*/, bool& /*sRGB*/)
{
    throw love::Exception("Compressed image parsing is not implemented for this format backend.");
}
