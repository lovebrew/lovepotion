#include "objects/transform/transform.h"

using namespace love;

love::Type Transform::type("Transform", &Object::type);

Transform::Transform() : matrix(), inverseDirty(true), inverseMatrix()
{}

Transform::Transform(const Matrix4& m) : matrix(m), inverseDirty(true), inverseMatrix()
{}

Transform::Transform(float x, float y, float a, float sx, float sy, float ox, float oy, float kx,
                     float ky) :
    matrix(x, y, a, sx, sy, ox, oy, kx, ky),
    inverseDirty(true),
    inverseMatrix()
{}

Transform* Transform::Clone()
{
    return new Transform(*this);
}

Transform* Transform::Inverse()
{
    return new Transform(this->GetInverseMatrix());
}

void Transform::Apply(Transform* other)
{
    this->matrix *= other->GetMatrix();
    this->inverseDirty = true;
}

void Transform::Translate(float x, float y)
{
    this->matrix.Translate(x, y);
    this->inverseDirty = true;
}

void Transform::Rotate(float angle)
{
    this->matrix.Rotate(angle);
    this->inverseDirty = true;
}

void Transform::Scale(float x, float y)
{
    this->matrix.Scale(x, y);
    this->inverseDirty = true;
}

void Transform::Shear(float x, float y)
{
    this->matrix.Shear(x, y);
    this->inverseDirty = true;
}

void Transform::Reset()
{
    this->matrix.SetIdentity();
    this->inverseDirty = true;
}

void Transform::SetTransformation(float x, float y, float a, float sx, float sy, float ox, float oy,
                                  float kx, float ky)
{
    this->matrix.SetTransformation(x, y, a, sx, sy, ox, oy, kx, ky);
    this->inverseDirty = true;
}

love::Vector2 Transform::TransformPoint(love::Vector2 p) const
{
    love::Vector2 result;
    this->matrix.TransformXY(&result, &p, 1);

    return result;
}

love::Vector2 Transform::InverseTransformPoint(love::Vector2 p)
{
    love::Vector2 result;
    this->GetInverseMatrix().TransformXY(&result, &p, 1);

    return result;
}

const Matrix4& Transform::GetMatrix() const
{
    return this->matrix;
}

void Transform::SetMatrix(const Matrix4& m)
{
    this->matrix       = m;
    this->inverseDirty = true;
}

bool Transform::GetConstant(const char* in, MatrixLayout& out)
{
    return matrixLayouts.Find(in, out);
}

bool Transform::GetConstant(MatrixLayout in, const char*& out)
{
    return matrixLayouts.Find(in, out);
}

std::vector<const char*> Transform::GetConstants(MatrixLayout)
{
    return matrixLayouts.GetNames();
}

// clang-format off
constexpr StringMap<Transform::MatrixLayout, Transform::MATRIX_MAX_ENUM>::Entry matrixLayoutEntries[] =
{
    { "row",    Transform::MatrixLayout::MATRIX_ROW_MAJOR    },
    { "column", Transform::MatrixLayout::MATRIX_COLUMN_MAJOR },
};

constinit const StringMap<Transform::MatrixLayout, Transform::MATRIX_MAX_ENUM> Transform::matrixLayouts(matrixLayoutEntries);
// clang-format on
