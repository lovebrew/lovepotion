#include <citro2d.h>

#include "objects/spritebatch/spritebatch.h"

#include "modules/graphics/graphics.h"
#include "objects/texture/texture.h"

#include "common/exception.h"

using namespace love;

SpriteBatch::SpriteBatch(Graphics* graphics, love::Texture* texture, int size) :
    common::SpriteBatch(graphics, texture, size)
{
    this->buffer.resize(size);
}

SpriteBatch::~SpriteBatch()
{
    this->buffer.clear();
}

void SpriteBatch::Flush()
{
    this->buffer.clear();
    this->buffer.resize(this->size);
}

int SpriteBatch::Add(Quad* quad, const Matrix4& matrix, int index)
{
    if (index < -1 || index >= this->size)
        throw love::Exception("Invalid sprite index: %d", index + 1);

    if (index == -1 && this->next >= this->size)
        this->SetBufferSize(this->size * 2);

    /* viewport of the Quad object */
    const Quad::Viewport v = quad->GetViewport();

    /* C2D_Image from the StrongRef<Texture> */
    const C2D_Image& handle = this->texture->GetHandle();

    /* calculate the SubTexture from the Quad::Viewport and handle's C3D_Tex */
    /* this is also the same as in Texture::Draw */
    Tex3DS_SubTexture tv = quad->CalculateTex3DSViewport(v, handle.tex);

    size_t offset = (index == -1) ? this->next : index;

    BufferInfo buffInfo {};

    buffInfo.subTex    = tv;
    buffInfo.transform = matrix;
    buffInfo.empty     = false;

    this->buffer[offset] = buffInfo;

    if (index == -1)
        return this->next++;

    return index;
}

void SpriteBatch::Draw(Graphics* graphics, const Matrix4& localTransform)
{
    if (this->next == 0)
        return;

    Matrix4 world(graphics->GetTransform(), localTransform);

    int counter = 0;
    int start   = std::clamp(this->rangeStart, 0, this->next - 1);

    int count = this->next;
    if (this->rangeCount > 0)
        count = std::min(count, this->rangeCount);

    if (count <= 0)
        return;

    C2D_Image image = this->texture->GetHandle();

    for (const auto& buffInfo : this->buffer)
    {
        if (buffInfo.empty)
            continue;

        image.subtex = &buffInfo.subTex;

        // Multiply the current and local transforms
        Matrix4 t(world, buffInfo.transform);

        C2D_DrawParams params;
        params.pos    = { 0.0f, 0.0f, (float)buffInfo.subTex.width, (float)buffInfo.subTex.height };
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
