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
    this->colorMemory = dk3d.GetImages()->allocate(layoutColorBuffer.getSize(), layoutColorBuffer.getAlignment());
    this->colorBuffer.initialize(layoutColorBuffer, this->colorMemory.getMemBlock(), this->colorMemory.getOffset());

    // LOVE sets up a Canvas to be transparent black
    memset(this->colorMemory.getCpuAddr(), 0, this->colorMemory.getSize());

    // Register the texture handle for the descriptor
    dk3d.RegisterResHandle(this->descriptor, this);
}

Canvas::~Canvas()
{
    colorMemory.destroy();
    dk3d.UnRegisterResHandle(this);
}

void Canvas::Draw(Graphics * gfx, Quad * quad, const Matrix4 & localTransform)
{
    // if (gfx->IsCanvasActive(this))
    //     throw love::Exception("Cannot render a Canvas to itself!");

    Texture::Draw(gfx, quad, localTransform);
}

void Canvas::Clear(const Colorf & color)
{}