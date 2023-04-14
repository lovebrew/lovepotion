#include <modules/image/imagemodule.hpp>

using namespace love;

ImageModule::~ImageModule()
{
    for (auto* handler : this->formatHandlers)
        handler->Release();
}

ImageData<Console::Which>* ImageModule::NewImageData(Data* data) const
{
    return new ImageData<Console::Which>(data);
}

ImageData<Console::Which>* ImageModule::NewImageData(int width, int height,
                                                     PixelFormat format) const
{
    return new ImageData<Console::Which>(width, height, format);
}

ImageData<Console::Which>* ImageModule::NewImageData(int width, int height, PixelFormat format,
                                                     void* data, bool own) const
{
    return new ImageData<Console::Which>(width, height, format, data, own);
}

const std::list<love::FormatHandler*>& ImageModule::GetFormatHandlers() const
{
    return this->formatHandlers;
}

bool ImageModule::IsCompressed(Data* data) const
{
    for (auto* handler : this->formatHandlers)
    {
        if (handler->CanParseCompressed(data))
            return true;
    }

    return false;
}
