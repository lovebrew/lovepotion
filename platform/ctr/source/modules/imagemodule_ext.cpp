#include <modules/image/imagemodule.hpp>

using namespace love;

ImageModule::ImageModule()
{
    this->formatHandlers.push_back(new T3XHandler());
}
