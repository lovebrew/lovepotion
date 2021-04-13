#include "objects/canvas/canvasc.h"

using namespace love::common;

love::Type Canvas::type("Canvas", &Texture::type);

Canvas::Canvas(const Settings& settings) : Texture(TEXTURE_2D), settings(settings)
{
    this->width  = settings.width;
    this->height = settings.height;

    this->InitQuad();
}