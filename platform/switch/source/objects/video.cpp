#include "objects/video/video.h"

#include "modules/graphics/graphics.h"
#include "objects/videostream/theorastream.h"

#include "deko3d/deko.h"

using namespace love;

Video::Video(Graphics* graphics, VideoStream* stream, float dpiScale) :
    common::Video(graphics, stream, dpiScale)
{
    std::fill_n(this->vertices, 4, vertex::Vertex {});

    this->vertices[0] = { .position = { 0, 0 }, .color = { 1, 1, 1, 1 }, .texcoord = { 0, 0 } };

    this->vertices[1] = { .position = { 0, (float)this->height },
                          .color    = { 1, 1, 1, 1 },
                          .texcoord = { 0, 1 } };

    this->vertices[2] = { .position = { (float)this->width, (float)this->height },
                          .color    = { 1, 1, 1, 1 },
                          .texcoord = { 1, 1 } };

    this->vertices[3] = { .position = { (float)this->width, 0 },
                          .color    = { 1, 1, 1, 1 },
                          .texcoord = { 1, 0 } };

    auto frame = (const TheoraStream::Frame*)this->stream->GetFrontBuffer();

    int widths[3]  = { frame->yw, frame->cw, frame->cw };
    int heights[3] = { frame->yh, frame->ch, frame->ch };

    const uint8_t* data[3] = { frame->yPlane, frame->cbPlane, frame->crPlane };

    Texture::Wrap wrap;
    for (size_t index = 0; index < 3; index++)
    {
        Image* image = graphics->NewImage(Texture::TEXTURE_2D, PIXELFORMAT_R8, widths[index],
                                          heights[index], 1);

        image->SetFilter(this->filter);
        image->SetWrap(wrap);

        size_t formatSize = GetPixelFormatSize(PIXELFORMAT_R8);
        size_t size       = widths[index] * heights[index] * formatSize;

        Rect rect = { 0, 0, widths[index], heights[index] };
        image->ReplacePixels(data[index], size, rect);

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

        int widths[3]  = { frame->yw, frame->cw, frame->cw };
        int heights[3] = { frame->yh, frame->ch, frame->ch };

        const uint8_t* data[3] = { frame->yPlane, frame->cbPlane, frame->crPlane };

        for (int i = 0; i < 3; i++)
        {
            size_t formatSize = GetPixelFormatSize(PIXELFORMAT_R8);

            size_t size = widths[i] * heights[i] * formatSize;

            Rect rect = { 0, 0, widths[i], heights[i] };
            images[i]->ReplacePixels(data[i], size, rect);
        }
    }
}

void Video::Draw(Graphics* graphics, const Matrix4& localTransform)
{
    this->Update();

    const Matrix4& tm  = graphics->GetTransform();
    bool is2D          = tm.IsAffine2DTransform();
    const Colorf color = graphics->GetColor();

    Matrix4 t(tm, localTransform);

    vertex::Vertex vertexData[4];
    std::fill_n(vertexData, 4, vertex::Vertex {});

    Vector2 transformed[4];
    std::fill_n(transformed, 4, Vector2 {});

    Vector2 positions[4];

    std::fill_n(positions, 4, Vector2 {});
    for (size_t index = 0; index < 4; index++)
        positions[index] =
            Vector2(this->vertices[index].position[0], this->vertices[index].position[1]);

    if (is2D)
        t.TransformXY(transformed, positions, 4);

    DkResHandle handles[3] = { this->images[0]->GetHandle(), this->images[1]->GetHandle(),
                               this->images[2]->GetHandle() };

    for (size_t i = 0; i < 4; i++)
    {
        vertexData[i] = { { transformed[i].x, transformed[i].y, 0.0f },
                          { color.r, color.g, color.b, color.a },
                          { vertex::normto16t(this->vertices[i].texcoord[0]),
                            vertex::normto16t(this->vertices[i].texcoord[1]) } };
    }

    ::deko3d::Instance().RenderVideo(handles, vertexData, 4);
}
