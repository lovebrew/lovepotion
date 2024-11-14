#include "modules/image/Image.hpp"

#if !defined(__3DS__)
    #include "modules/image/magpie/ASTCHandler.hpp"
    #include "modules/image/magpie/JPGHandler.hpp"
    #include "modules/image/magpie/KTXHandler.hpp"
    #include "modules/image/magpie/PNGHandler.hpp"
    #include "modules/image/magpie/ddsHandler.hpp"
#else
    #include "modules/image/magpie/T3XHandler.hpp"
#endif

namespace love
{
    Image::Image() : Module(M_IMAGE, "love.image")
    {
        float16Init();

        this->formatHandlers = {
#if defined(__3DS__)
            new T3XHandler
#else
            new ASTCHandler,
            new JPGHandler,
            new KTXHandler,
            new PNGHandler,
            new DDSHandler
#endif
        };
    }

    Image::~Image()
    {
        for (FormatHandler* handler : this->formatHandlers)
            handler->release();
    }

    ImageData* Image::newImageData(Data* data) const
    {
        return new ImageData(data);
    }

    ImageData* Image::newImageData(int width, int height, PixelFormat format) const
    {
        return new ImageData(width, height, format);
    }

    ImageData* Image::newImageData(int width, int height, PixelFormat format, void* data, bool own) const
    {
        return new ImageData(width, height, format, data, own);
    }

    CompressedImageData* Image::newCompressedData(Data* data) const
    {
        return new CompressedImageData(this->formatHandlers, data);
    }

    bool Image::isCompressed(Data* data) const
    {
        for (FormatHandler* handler : this->formatHandlers)
        {
            if (handler->canParseCompressed(data))
                return true;
        }

        return false;
    }

    const std::list<FormatHandler*>& Image::getFormatHandlers() const
    {
        return this->formatHandlers;
    }

    ImageData* Image::newPastedImageData(ImageData* source, int x, int y, int width, int height) const
    {
        auto* result = this->newImageData(width, height, source->getFormat());

        try
        {
            result->paste(source, 0, 0, x, y, width, height);
        }
        catch (love::Exception&)
        {
            result->release();
            throw;
        }

        return result;
    }
} // namespace love
