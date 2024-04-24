#include "modules/math/Transform.hpp"

namespace love
{
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

    Transform::~Transform()
    {}

    Transform* Transform::clone()
    {
        return new Transform(*this);
    }

    Transform* Transform::inverse()
    {
        return new Transform(this->getInverseMatrix());
    }

    void Transform::apply(Transform* other)
    {
        this->matrix *= other->getMatrix();
        this->inverseDirty = true;
    }

    void Transform::translate(float x, float y)
    {
        this->matrix.translate(x, y);
        this->inverseDirty = true;
    }

    void Transform::rotate(float angle)
    {
        this->matrix.rotate(angle);
        this->inverseDirty = true;
    }

    void Transform::scale(float x, float y)
    {
        this->matrix.scale(x, y);
        this->inverseDirty = true;
    }

    void Transform::shear(float x, float y)
    {
        this->matrix.shear(x, y);
        this->inverseDirty = true;
    }

    void Transform::reset()
    {
        this->matrix.setIdentity();
        this->inverseDirty = true;
    }

    void Transform::setTransformation(float x, float y, float a, float sx, float sy, float ox, float oy,
                                      float kx, float ky)
    {
        this->matrix.setTransformation(x, y, a, sx, sy, ox, oy, kx, ky);
        this->inverseDirty = true;
    }

    Vector2 Transform::transformPoint(Vector2 point) const
    {
        Vector2 result {};
        this->matrix.transformXY(&result, &point, 1);

        return result;
    }

    Vector2 Transform::inverseTransformPoint(Vector2 point)
    {
        Vector2 result {};
        this->getInverseMatrix().transformXY(&result, &point, 1);

        return result;
    }

    const Matrix4& Transform::getMatrix() const
    {
        return this->matrix;
    }

    void Transform::setMatrix(const Matrix4& matrix)
    {
        this->matrix       = matrix;
        this->inverseDirty = true;
    }
} // namespace love
