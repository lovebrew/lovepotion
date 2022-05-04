#include "modules/image/imagemodule.h"

#include "objects/imagedata/handlers/jpghandler.h"
#include "objects/imagedata/handlers/pnghandler.h"
#include "objects/imagedata/handlers/t3xhandler.h"

#include "common/bidirectionalmap.h"
#include "objects/compressedimagedata/handlers/astchandler.h"
#include "objects/compressedimagedata/handlers/ddshandler.h"
#include "objects/compressedimagedata/handlers/pkmhandler.h"

using namespace love;

ImageModule::ImageModule()
{
    this->formatHandlers = {
#if not defined(__3DS__)
        new PNGHandler(),
        new JPGHandler(),
        new DDSHandler(),
        new PKMHandler(),
        new ASTCHandler(),
#endif
        new T3XHandler()
    };
}

ImageModule::~ImageModule()
{
    for (auto* handler : this->formatHandlers)
        handler->Release();
}

ImageData* ImageModule::NewImageData(Data* data)
{
    return new ImageData(data);
}

ImageData* ImageModule::NewImageData(int width, int height, PixelFormat format)
{
    return new ImageData(width, height, format);
}

ImageData* ImageModule::NewImageData(int width, int height, PixelFormat format, void* data,
                                     bool own)
{
    return new ImageData(width, height, format, data, own);
}

CompressedImageData* ImageModule::NewCompressedData(Data* data)
{
    return new CompressedImageData(this->formatHandlers, data);
}

bool ImageModule::IsCompressed(Data* data)
{
    for (FormatHandler* handler : formatHandlers)
    {
        if (handler->CanParseCompressed(data))
            return true;
    }

    return false;
}

const std::list<FormatHandler*>& ImageModule::GetFormatHandlers() const
{
    return this->formatHandlers;
}
