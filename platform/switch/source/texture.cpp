#include "common/runtime.h"
#include "objects/texture/texture.h"

#include "modules/graphics/graphics.h"

#include "common/vector.h"
#include "common/matrix.h"

#include "deko3d/deko.h"

using namespace love;

void Texture::Draw(Graphics * gfx, const Matrix4 & localTransform)
{
    this->Draw(gfx, this->quad, localTransform);
}

void Texture::Draw(Graphics * gfx, love::Quad * quad, const Matrix4 & localTransform)
{
    Quad::Viewport v = quad->GetViewport();

    const Vector2 * texcoords = quad->GetVertexTexCoords();

    const Matrix4 & tm = gfx->GetTransform();
    bool is2D = tm.IsAffine2DTransform();

    Matrix4 t(tm, localTransform);

    std::vector<vertex::Vertex> points;

    if (is2D)
    {
        Vector2 transformed[4];
        points = vertex::GenerateTextureFromVectors(transformed, texcoords, 4, gfx->GetColor());

        t.TransformXY(transformed, quad->GetVertexPositions(), 4);
    }

    dk3d.RenderTexture(this->handle, points.data(), 4 * sizeof(*points.data()), 4);
}