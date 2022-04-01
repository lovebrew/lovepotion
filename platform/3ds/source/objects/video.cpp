#include "objects/video/video.h"
#include "objects/videostream/theorastream.h"

#include "modules/graphics/graphics.h"

using namespace love;

Video::Video(Graphics* graphics, VideoStream* stream, float dpiScale) :
    common::Video(graphics, stream, dpiScale)
{
    this->stream->FillBackBuffer();

    auto frame = (const TheoraStream::Frame*)this->stream->GetFrontBuffer();
    Rect rect  = { 0, 0, frame->width, frame->height };

    /* initialize all these StrongReference<Image> items, so setFilter will be happy */
    for (int index = 0; index < 3; index++)
    {
        Image* image = graphics->NewImage(Texture::TEXTURE_2D, PixelFormat::PIXELFORMAT_RGB8,
                                          rect.w, rect.h, 1);

        image->ReplacePixels(frame->buffer->data, frame->buffer->size, rect);
        this->images[index].Set(image, Acquire::NORETAIN);
    }
}

Video::~Video()
{}

void Video::Update()
{
    bool buffersChanged = this->stream->SwapBuffers();
    this->stream->FillBackBuffer();

    if (buffersChanged)
    {
        auto frame = (const TheoraStream::Frame*)this->stream->GetFrontBuffer();
        Rect rect  = { 0, 0, frame->width, frame->height };

        for (int index = 0; index < 1; index++)
            this->images[index]->ReplacePixels(frame->buffer->data, frame->buffer->size, rect);
    }
}

void Video::Draw(Graphics* graphics, const Matrix4& localTransform)
{
    this->Update();

    this->images[0]->Draw(graphics, localTransform);
}
