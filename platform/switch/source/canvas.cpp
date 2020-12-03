#include "common/runtime.h"
#include "objects/canvas/canvas.h"

#include "deko3d/deko.h"

using namespace love;

Canvas::Canvas(const Canvas::Settings & settings) : common::Canvas(settings)
{
    // Create layout for the (multisampled) color buffer
    dk::ImageLayout layoutColorBuffer;
    dk::ImageLayoutMaker{dk3d.GetDevice()}
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_Usage2DEngine | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(this->width, this->height)
        .initialize(layoutColorBuffer);

    // Create the color buffer
    colorMemory = dk3d.GetImages()->allocate(layoutColorBuffer.getSize(), layoutColorBuffer.getAlignment());
    colorBuffer.initialize(layoutColorBuffer, colorMemory.getMemBlock(), colorMemory.getOffset());

    // LOVE sets up a Canvas to be transparent black
}

Canvas::~Canvas()
{
    colorMemory.destroy();
}

void Canvas::Draw(Graphics * gfx, Quad * quad, const Matrix4 & localTransform)
{
    // if (gfx->IsCanvasActive(this))
    //     throw love::Exception("Cannot render a Canvas to itself!");

    Texture::Draw(gfx, quad, localTransform);
}

void Canvas::Clear(const Colorf & color)
{}