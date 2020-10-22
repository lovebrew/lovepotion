#include "common/runtime.h"
#include "objects/texture/texture.h"

#include "modules/graphics/graphics.h"

using namespace love;

void Texture::Draw(Graphics * gfx, const Matrix4 & localTransform)
{
    this->Draw(gfx, this->quad, localTransform);
}

void Texture::Draw(Graphics * gfx, love::Quad * quad, const Matrix4 & localTransform)
{}