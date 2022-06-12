#include "objects/texture/texture.h"
#include "modules/graphics/graphics.h"

#include "common/matrix.h"
#include "common/vector.h"

#include "deko3d/deko.h"

using namespace love;

Texture::Texture(TextureType type) : common::Texture(type), texture()
{}

Texture::~Texture()
{
    ::deko3d::Instance().DeRegisterDescriptor(this->handle);
}

DkResHandle Texture::GetHandle()
{
    return this->handle;
}

void Texture::SetSamplerState(const SamplerState& state)
{
    this->samplerState = state;

    if (state.mipmapFilter != SamplerState::MIPMAP_FILTER_NONE && this->GetMipmapCount() == 1)
        this->samplerState.mipmapFilter = SamplerState::MIPMAP_FILTER_NONE;

    ::deko3d::Instance().SetSamplerState(this, this->samplerState);
}

void Texture::Draw(Graphics* graphics, const Matrix4& localTransform)
{
    this->Draw(graphics, this->quad, localTransform);
}

void Texture::Draw(Graphics* graphics, love::Quad* quad, const Matrix4& localTransform)
{
    const Matrix4& tm  = graphics->GetTransform();
    bool is2D          = tm.IsAffine2DTransform();
    const Colorf color = graphics->GetColor();

    Matrix4 t(tm, localTransform);

    DrawCommand command(Vertex::PRIMITIVE_QUADS, 4, { this->handle }, Shader::STANDARD_TEXTURE);

    if (is2D)
        t.TransformXY(command.GetPositions(), quad->GetVertexPositions(), 4);

    const Vector2* texCoords            = quad->GetVertexTexCoords();
    Vertex::PrimitiveVertex* vertexData = command.GetVertices();

    // clang-format off
    for (size_t index = 0; index < 4; index++)
    {
        vertexData[index] = {
            .position = { command.positions[index].x, command.positions[index].y, 0.0f },
            .color    = { color.r, color.g, color.b, color.a },
            .texcoord = { Vertex::normto16t(texCoords[index].x), Vertex::normto16t(texCoords[index].y) }
        };
    }
    // clang-format on

    ::deko3d::Instance().Render(command);
}
