#include "objects/canvas/canvas.h"

#include "deko3d/deko.h"

#include "modules/graphics/graphics.h"

using namespace love;

Canvas::Canvas(const Canvas::Settings& settings) : common::Canvas(settings), descriptor {}
{
    // Create layout for the (multisampled) color buffer
    dk::ImageLayout layoutColorBuffer;
    dk::ImageLayoutMaker { ::deko3d::Instance().GetDevice() }
        .setFlags(DkImageFlags_UsageRender | DkImageFlags_HwCompression)
        .setFormat(DkImageFormat_RGBA8_Unorm)
        .setDimensions(this->width, this->height)
        .initialize(layoutColorBuffer);

    // Create the color buffer
    this->colorMemory = ::deko3d::Instance().GetImages().allocate(layoutColorBuffer.getSize(),
                                                                  layoutColorBuffer.getAlignment());
    this->colorBuffer.initialize(layoutColorBuffer, this->colorMemory.getMemBlock(),
                                 this->colorMemory.getOffset());

    // Clear to transparent black
    ::deko3d::Instance().BindFramebuffer(this);
    ::deko3d::Instance().ClearColor({ 0, 0, 0, 0 });
    ::deko3d::Instance().BindFramebuffer();

    dk::ImageView view { this->colorBuffer };
    this->descriptor.initialize(view);

    // Register the texture handle for the descriptor
    this->handle = ::deko3d::Instance().RegisterResHandle(this->texture.getDescriptor());
}

Canvas::~Canvas()
{
    this->colorMemory.destroy();
}

void Canvas::Draw(Graphics* gfx, Quad* quad, const Matrix4& localTransform)
{
    if (gfx->IsCanvasActive(this))
        throw love::Exception("Cannot render a Canvas to itself!");

    Texture::Draw(gfx, quad, localTransform);
}
