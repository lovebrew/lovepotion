#include <objects/transform/transform.hpp>

using namespace love;

Type Transform::type("Transform", &Object::type);

Transform::Transform() : matrix(), inverseDirty(true), inverseMatrix()
{}

Transform::Transform(const Matrix4& matrix) : matrix(matrix), inverseDirty(true), inverseMatrix()
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

Vector2 Transform::TransformPoint(Vector2 point) const
{
    Vector2 result;
    this->matrix.TransformXY(std::views::single(result), std::views::single(point));

    return result;
}

Vector2 Transform::InverseTransformPoint(Vector2 point)
{
    Vector2 result;
    this->GetInverseMatrix().TransformXY(std::views::single(result), std::views::single(point));

    return result;
}

Matrix4& Transform::GetMatrix()
{
    return this->matrix;
}

void Transform::SetMatrix(const Matrix4& matrix)
{
    this->matrix       = matrix;
    this->inverseDirty = true;
}
