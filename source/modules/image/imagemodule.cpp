#include <modules/image/imagemodule.hpp>

using namespace love;

ImageModule::ImageModule()
{
    if (Console::Is(Console::CTR))
        this->formatHandlers.push_back(new T3XHandler());
}

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
