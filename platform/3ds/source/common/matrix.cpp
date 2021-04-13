#include "common/matrix.h"

using namespace love;

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, C3D_Mtx& c)
{
    Mtx_Multiply(&c, &a.matrix, &b.matrix);
}

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, Matrix4& c)
{
    Multiply(a, b, c.matrix);
}

Matrix4::Matrix4()
{
    this->SetIdentity();
}

Matrix4::Matrix4(const C3D_Mtx& a)
{
    Mtx_Copy(&this->matrix, &a);
}

bool Matrix4::IsAffine2DTransform() const
{
    /*
        return fabsf(this->matrix[2] + this->matrix[3] + this->matrix[6] + this->matrix[7] +
                 this->matrix[8] + this->matrix[9] + this->matrix[11] + this->matrix[14]) < 0.00001f
       && fabsf(this->matrix[10] + this->matrix[15] - 2.0f) < 0.00001f;
    */
    return true; //?
}

Matrix4::Matrix4(const Matrix4& a, const Matrix4& b)
{
    Matrix4::Multiply(a, b, this->matrix);
}

Matrix4::Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                 float ky)
{
    this->SetTransformation(x, y, angle, sx, sy, ox, oy, ky, ky);
}

void Matrix4::operator*=(const Matrix4& m)
{
    Matrix4::Multiply(*this, m, this->matrix);
}

Matrix4 Matrix4::operator*(const Matrix4& m) const
{
    return Matrix4(*this, m);
}

void Matrix4::SetIdentity()
{
    Mtx_Identity(&this->matrix);
}

void Matrix4::SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y)
{
    Mtx_Zeros(&this->matrix);

    this->matrix.r[2].z = this->matrix.r[3].w = 1.0f;

    this->matrix.r[0].x = t00;
    this->matrix.r[1].x = t10;

    this->matrix.r[0].y = t01;
    this->matrix.r[1].y = t11;

    this->matrix.r[0].w = x;
    this->matrix.r[1].w = y;
}

void Matrix4::SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                                float oy, float kx, float ky)
{
    Mtx_Zeros(&this->matrix);

    /*
    ** Note that the offsets are applied before rotation, scaling, or shearing;
    ** scaling and shearing are applied before rotation.
    */
    float c = cosf(angle), s = sinf(angle);

    this->matrix.r[2].z = this->matrix.r[3].w = 1.0f;

    this->matrix.r[0].x = c * sx - ky * s * sy; // = a
    this->matrix.r[1].x = s * sx + ky * c * sy; // = b

    this->matrix.r[0].y = kx * c * sx - s * sy; // = c
    this->matrix.r[1].y = kx * s * sx + c * sy; // = d

    this->matrix.r[0].w = x - ox * this->matrix.r[0].x - oy * this->matrix.r[0].y; // = c
    this->matrix.r[1].w = y - ox * this->matrix.r[1].x - oy * this->matrix.r[1].y; // = d
}

void Matrix4::Translate(float x, float y)
{
    Mtx_Translate(&this->matrix, x, y, 0.0f, true);
}

void Matrix4::Rotate(float r)
{
    Mtx_RotateZ(&this->matrix, r, true);
}

void Matrix4::Scale(float sx, float sy)
{
    Mtx_Scale(&this->matrix, sx, sy, 1.0f);
}

void Matrix4::GetApproximateScale(float& sx, float& sy) const
{
    sx = sqrtf(this->matrix.r[0].x * this->matrix.r[0].x +
               this->matrix.r[0].y * this->matrix.r[0].y);
    sy = sqrtf(this->matrix.r[1].x * this->matrix.r[1].x +
               this->matrix.r[1].y * this->matrix.r[1].y);
}

void Matrix4::Shear(float kx, float ky)
{
    C3D_Mtx mtx;
    Mtx_Identity(&mtx);

    mtx.r[0].y = kx;
    mtx.r[1].x = ky;

    Mtx_Multiply(&this->matrix, &this->matrix, &mtx);
}

const C3D_Mtx& Matrix4::GetElements() const
{
    return this->matrix;
}

void Matrix4::TransformXY()
{
    C2D_ViewRestore(&this->matrix);
}

void Matrix4::TransformXY(const Elements& elements)
{
    C2D_ViewRestore(&elements);
}

Matrix4 Matrix4::Inverse() const
{
    Matrix4 inv;
    Mtx_Copy(&inv.matrix, &this->matrix);

    Mtx_Inverse(&inv.matrix);

    return inv;
}

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float near, float far)
{
    Matrix4 ortho;
    Mtx_Ortho(&ortho.matrix, left, right, bottom, top, near, far, true);

    return ortho;
}