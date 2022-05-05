#include "objects/spritebatch/spritebatch.h"

#include "modules/graphics/graphics.h"

#include "objects/quad/quad.h"
#include "objects/texture/texture.h"

#include "common/exception.h"

using namespace love;

SpriteBatch::SpriteBatch(Graphics* graphics, love::Texture* texture, int size) :
    common::SpriteBatch(graphics, texture, size)
{
    this->buffer.reserve(size);
}

SpriteBatch::~SpriteBatch()
{
    this->buffer.clear();
}

void SpriteBatch::Flush()
{
    this->buffer.clear();
    this->buffer.reserve(this->size);
}

int SpriteBatch::Add(Quad* quad, const Matrix4& matrix, int index)
{
    if (index < -1 || index >= this->size)
        throw love::Exception("Invalid sprite index: %d", index + 1);

    if (index == -1 && this->next >= this->size)
        this->SetBufferSize(this->size * 2);

    const Quad::Viewport v = quad->GetViewport();

    const C2D_Image& handle = this->texture->GetHandle();
    Tex3DS_SubTexture tv    = quad->CalculateTex3DSViewport(v, handle.tex);

    size_t offset = (index == -1) ? this->next : index;

    this->buffer[offset] = BufferInfo { .subTex = tv, .viewport = v, .transform = matrix };

    if (index == -1)
        return this->next++;

    return index;
}

void SpriteBatch::Draw(Graphics* graphics, const Matrix4& localTransform)
{
    if (this->next == 0)
        return;

    Matrix4 world(graphics->GetTransform(), localTransform);

    for (const auto& buffInfo : this->buffer)
    {
        C2D_Image image = this->texture->GetHandle();
        image.subtex    = &buffInfo.subTex;

        // Multiply the current and local transforms
        Matrix4 t(world, buffInfo.transform);

        C2D_DrawParams params;
        params.pos    = { 0.0f, 0.0f, (float)buffInfo.viewport.w, (float)buffInfo.viewport.h };
        params.depth  = Graphics::CURRENT_DEPTH;
        params.angle  = 0.0f;
        params.center = { 0.0f, 0.0f };

        C2D_ViewRestore(&t.GetElements());

        Colorf color = graphics->GetColor();
        if (this->colorActive)
            color = this->color;

        C2D_ImageTint tint;
        C2D_PlainImageTint(&tint, C2D_Color32f(color.r, color.g, color.b, color.a), 1);

        C2D_DrawImage(image, &params, &tint);
    }
}

void SpriteBatch::SetBufferSize(int newSize)
{
    if (newSize <= 0)
        throw love::Exception("Invalid SpriteBatch size.");

    if (newSize == this->size)
        return;

    int newNext = std::min(this->next, newSize);
    std::vector<SpriteBatch::BufferInfo> newBuffer(this->buffer);

    this->buffer = newBuffer;

    this->size = newSize;
    this->next = newNext;
}
