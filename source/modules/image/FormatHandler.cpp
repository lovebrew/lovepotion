#include "common/Exception.hpp"

#include "modules/image/FormatHandler.hpp"

namespace love
{
    FormatHandler::FormatHandler()
    {}

    FormatHandler::~FormatHandler()
    {}

    bool FormatHandler::canDecode(Data*) const
    {
        return false;
    }

    bool FormatHandler::canEncode(PixelFormat, EncodedFormat) const
    {
        return false;
    }

    FormatHandler::DecodedImage FormatHandler::decode(Data*) const
    {
        throw love::Exception("Image decoding is not implemented for this format.");
    }

    FormatHandler::EncodedImage FormatHandler::encode(const DecodedImage&, EncodedFormat) const
    {
        throw love::Exception("Image encoding is not implemented for this format.");
    }

    bool FormatHandler::canParseCompressed(Data*) const
    {
        return false;
    }

    StrongRef<ByteData> FormatHandler::parseCompressed(Data*, CompressedSlices&, PixelFormat&) const
    {
        throw love::Exception("Compressed image parsing is not implemented for this format.");
    }

    void FormatHandler::freeRawPixels(uint8_t* memory)
    {
        delete[] memory;
    }

    void FormatHandler::freeEncodedImage(uint8_t* memory)
    {
        delete[] memory;
    }
} // namespace love
