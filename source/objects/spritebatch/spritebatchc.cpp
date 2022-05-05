#include "objects/spritebatch/spritebatchc.h"

#include "modules/graphics/graphics.h"

#include "objects/quad/quad.h"
#include "objects/texture/texture.h"

#include <algorithm>

using namespace love::common;

love::Type SpriteBatch::type("SpriteBatch", &Drawable::type);

SpriteBatch::SpriteBatch(Graphics* graphics, love::Texture* texture, int size) :
    texture(texture),
    size(size),
    next(0),
    color(1, 1, 1, 1),
    colorActive(false),
    rangeStart(-1),
    rangeCount(-1)
{
    if (size <= 0)
        throw love::Exception("Invalid SpriteBatch size.");

    if (texture == nullptr)
        throw love::Exception("A Texture must be used when creating a SpriteBatch.");
}

SpriteBatch::~SpriteBatch()
{}

int SpriteBatch::Add(const Matrix4& matrix, int index)
{
    return this->Add(this->texture->GetQuad(), matrix, index);
}

void SpriteBatch::Clear()
{
    this->next = 0;
}

void SpriteBatch::SetTexture(love::Texture* newTexture)
{
    if (this->texture->GetTextureType() != newTexture->GetTextureType())
        throw love::Exception(
            "New Texture must have the same texture type as the SpriteBatch's previous Texture.");

    this->texture.Set(newTexture);
}

love::Texture* SpriteBatch::GetTexture() const
{
    return this->texture.Get();
}

void SpriteBatch::SetColor(const Colorf& color)
{
    this->colorActive = true;
    this->color       = color;
}

void SpriteBatch::SetColor()
{
    this->colorActive = false;
    this->color       = Colorf(1, 1, 1, 1);
}

Colorf SpriteBatch::GetColor(bool& active) const
{
    active = this->colorActive;

    return this->color;
}

int SpriteBatch::GetCount() const
{
    return this->next;
}

int SpriteBatch::GetBufferSize() const
{
    return this->size;
}

void SpriteBatch::SetDrawRange(int start, int count)
{
    if (start < 0 || count <= 0)
        throw love::Exception("Invalid draw range.");

    this->rangeStart = start;
    this->rangeCount = count;
}

void SpriteBatch::SetDrawRange()
{
    this->rangeStart = this->rangeCount = -1;
}

bool SpriteBatch::GetDrawRange(int& start, int& count) const
{
    if (this->rangeStart < 0 || this->rangeCount <= 0)
        return false;

    start = this->rangeStart;
    count = this->rangeCount;

    return true;
}
