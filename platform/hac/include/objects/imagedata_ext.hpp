#pragma once

#include <objects/imagedata/imagedata.tcc>

#include <common/math.hpp>
#include <common/strongreference.hpp>

#include <utilities/formathandler/types/jpghandler.hpp>
#include <utilities/formathandler/types/pnghandler.hpp>

namespace love
{
    template<>
    class ImageData<Console::HAC> : public ImageData<Console::ALL>
    {
      public:
        ImageData(Data* data);

        using ImageData<Console::ALL>::ImageData;

        virtual ~ImageData();

        ImageData* Clone() const override
        {
            return new ImageData(*this);
        }

        void Paste(ImageData* data, int x, int y, Rect& source);

        void SetPixel(int x, int y, const Color& color);

        void GetPixel(int x, int y, Color& color);

        Color GetPixel(int x, int y);

        FileData* Encode(FormatHandler::EncodedFormat format, std::string_view filename,
                         bool writeFile) const;

      private:
        void Create(int width, int height, PixelFormat format, void* data);

        void Decode(Data* data);

        StrongReference<FormatHandler> decoder;
    };
} // namespace love
