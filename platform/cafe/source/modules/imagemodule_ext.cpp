#include <modules/image/imagemodule.hpp>

using namespace love;

ImageModule::ImageModule()
{
    this->formatHandlers = { new JPGHandler(), new PNGHandler() };
}
