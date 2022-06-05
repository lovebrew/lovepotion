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

void Texture::SetHandle(DkResHandle handle)
{
    this->handle = handle;
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

void Texture::Draw(Graphics* gfx, const Matrix4& localTransform)
{
    this->Draw(gfx, this->quad, localTransform);
}

void Texture::Draw(Graphics* gfx, love::Quad* quad, const Matrix4& localTransform)
{
    const Matrix4& tm  = gfx->GetTransform();
    bool is2D          = tm.IsAffine2DTransform();
    const Colorf color = gfx->GetColor();

    Matrix4 t(tm, localTransform);

    /* zero out a new vertex data thing */
    Vertex::PrimitiveVertex vertexData[TEXTURE_QUAD_POINT_COUNT];
    std::fill_n(vertexData, TEXTURE_QUAD_POINT_COUNT, Vertex::PrimitiveVertex {});

    Vector2 transformed[TEXTURE_QUAD_POINT_COUNT];
    std::fill_n(transformed, TEXTURE_QUAD_POINT_COUNT, Vector2 {});

    if (is2D)
        t.TransformXY(transformed, quad->GetVertexPositions(), TEXTURE_QUAD_POINT_COUNT);

    const Vector2* texCoords = quad->GetVertexTexCoords();

    for (size_t i = 0; i < TEXTURE_QUAD_POINT_COUNT; i++)
    {
        vertexData[i] = { { transformed[i].x, transformed[i].y, 0.0f },
                          { color.r, color.g, color.b, color.a },
                          { Vertex::normto16t(texCoords[i].x),
                            Vertex::normto16t(texCoords[i].y) } };
    }

    ::deko3d::Instance().RenderTexture(this->handle, vertexData, TEXTURE_QUAD_POINT_COUNT);
}
