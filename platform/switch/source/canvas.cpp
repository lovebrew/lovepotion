#include "common/runtime.h"
#include "objects/canvas/canvas.h"

#include "deko3d/deko.h"

#include "modules/graphics/graphics.h"

using namespace love;

Canvas::Canvas(const Canvas::Settings & settings) : common::Canvas(settings),
                                                    descriptor{}
{
    // Create layout for the (multisampled) color buffer
    dk::ImageLayout layoutColorBuffer;
    dk::ImageLayoutMaker{dk3d.GetDevice()}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_Usage2DEngine | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(this->width, this->height)
        .initialize(layoutColorBuffer);

    // Create the color buffer
    this->colorMemory = dk3d.GetImages()->allocate(layoutColorBuffer.getSize(), layoutColorBuffer.getAlignment());
    this->colorBuffer.initialize(layoutColorBuffer, this->colorMemory.getMemBlock(), this->colorMemory.getOffset());

    dk::ImageView view { this->colorBuffer };
    this->descriptor.initialize(view);

    // Register the texture handle for the descriptor
    dk3d.RegisterResHandle(this->descriptor, this);
}

Canvas::~Canvas()
{
    this->colorMemory.destroy();
    dk3d.UnRegisterResHandle(this);
}

void Canvas::SetAsTarget()
{}

void Canvas::Draw(Graphics * gfx, Quad * quad, const Matrix4 & localTransform)
{
    if (gfx->IsCanvasActive(this))
        throw love::Exception("Cannot render a Canvas to itself!");

    Texture::Draw(gfx, quad, localTransform);
}

void Canvas::Clear(const Colorf & color)
{}