#include "objects/video/video.h"
#include "objects/videostream/theorastream.h"

#include "modules/graphics/graphics.h"

using namespace love;

Video::Video(Graphics* graphics, VideoStream* stream, float dpiScale) :
    common::Video(graphics, stream, dpiScale)
{}

Video::~Video()
{}

void Video::Update()
{}

void Video::Draw(Graphics* graphics, const Matrix4& localTransform)
{
    bool changed = this->stream->SwapBuffers();
    this->stream->FillBackBuffer();

    if (changed)
    {
        auto frame = (TheoraStream::Frame*)this->stream->GetFrontBuffer();

        // Multiply the current and local transforms
        Matrix4 t(graphics->GetTransform(), localTransform);

        C2D_DrawParams params;

        params.pos = { 0.0f, 0.0f, (float)frame->image.subtex->width,
                       (float)frame->image.subtex->height };

        params.depth  = Graphics::CURRENT_DEPTH;
        params.angle  = 0.0f;
        params.center = { 0.0f, 0.0f };

        C2D_ViewRestore(&t.GetElements());

        C2D_ImageTint tint;
        Colorf color = graphics->GetColor();

        C2D_PlainImageTint(&tint, C2D_Color32f(color.r, color.g, color.b, color.a), 1);
        C2D_DrawImage(frame->image, &params, &tint);
    }
}
