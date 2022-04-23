#include "objects/video/videoc.h"

using namespace love::common;

love::Type Video::type("Video", &Drawable::type);

Video::Video(Graphics* graphics, VideoStream* stream, float dpiScale) :
    stream(stream),
    width(stream->GetWidth() / dpiScale),
    height(stream->GetHeight() / dpiScale),
    filter(Texture::defaultFilter)
{
    this->filter.mipmap = Texture::FILTER_NONE;
    this->stream->FillBackBuffer();
}

Video::~Video()
{
    if (this->source)
        this->source->Stop();
}

love::VideoStream* Video::GetStream()
{
    return this->stream;
}

love::Source* Video::GetSource()
{
    return this->source;
}

void Video::SetSource(love::Source* source)
{
    this->source = source;
}

int Video::GetWidth() const
{
    return this->width;
}

int Video::GetHeight() const
{
    return this->height;
}

int Video::GetPixelWidth() const
{
    return this->stream->GetWidth();
}

int Video::GetPixelHeight() const
{
    return this->stream->GetHeight();
}

void Video::SetFilter(const Texture::Filter& filter)
{
    for (const auto& image : this->images)
        image->SetFilter(filter);

    this->filter = filter;
}

const Texture::Filter& Video::GetFilter() const
{
    return this->filter;
}
