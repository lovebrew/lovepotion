#pragma once

#include <objects/imagedata/imagedata.tcc>

#include <common/math.hpp>
#include <common/strongreference.hpp>

namespace love
{
    template<>
    class ImageData<Console::CTR> : public ImageData<Console::ALL>
    {
      public:
        using ImageData<Console::ALL>::ImageData;

        virtual ~ImageData()
        {}

        ImageData* Clone() const override
        {
            return new ImageData(*this);
        }

        void Paste(ImageData* data, int x, int y, Rect& source);

        FileData* Encode(FormatHandler::EncodedFormat format, std::string_view filename,
                         bool writeFile) const
        {
            throw love::Exception("This platform does not support encoding images.");
            return nullptr;
        }
    };
} // namespace love
