#include "objects/canvas/canvasc.h"

using namespace love::common;

love::Type Canvas::type("Canvas", &Texture::type);
int Canvas::canvasCount = 0;

Canvas::Canvas(const Settings& settings) : Texture(TEXTURE_2D), settings(settings)
{
    this->width  = settings.width;
    this->height = settings.height;

    this->InitQuad();
    Canvas::canvasCount++;
}

Canvas::~Canvas()
{
    Canvas::canvasCount--;
}
