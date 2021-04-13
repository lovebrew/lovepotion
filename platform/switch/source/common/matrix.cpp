#include "common/matrix.h"

#include <arm_neon.h>
#include <string.h>

using namespace love;

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, Elements& t)
{
    float32x4_t cola1 = vld1q_f32(&a.matrix[0]);
    float32x4_t cola2 = vld1q_f32(&a.matrix[4]);
    float32x4_t cola3 = vld1q_f32(&a.matrix[8]);
    float32x4_t cola4 = vld1q_f32(&a.matrix[12]);

    float32x4_t col1 = vmulq_n_f32(cola1, b.matrix[0]);
    col1             = vmlaq_n_f32(col1, cola2, b.matrix[1]);
    col1             = vmlaq_n_f32(col1, cola3, b.matrix[2]);
    col1             = vmlaq_n_f32(col1, cola4, b.matrix[3]);

    float32x4_t col2 = vmulq_n_f32(cola1, b.matrix[4]);
    col2             = vmlaq_n_f32(col2, cola2, b.matrix[5]);
    col2             = vmlaq_n_f32(col2, cola3, b.matrix[6]);
    col2             = vmlaq_n_f32(col2, cola4, b.matrix[7]);

    float32x4_t col3 = vmulq_n_f32(cola1, b.matrix[8]);
    col3             = vmlaq_n_f32(col3, cola2, b.matrix[9]);
    col3             = vmlaq_n_f32(col3, cola3, b.matrix[10]);
    col3             = vmlaq_n_f32(col3, cola4, b.matrix[11]);

    float32x4_t col4 = vmulq_n_f32(cola1, b.matrix[12]);
    col4             = vmlaq_n_f32(col4, cola2, b.matrix[13]);
    col4             = vmlaq_n_f32(col4, cola3, b.matrix[14]);
    col4             = vmlaq_n_f32(col4, cola4, b.matrix[15]);

    vst1q_f32(&t[0], col1);
    vst1q_f32(&t[4], col2);
    vst1q_f32(&t[8], col3);
    vst1q_f32(&t[12], col4);
}

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, Matrix4& t)
{
    Matrix4::Multiply(a, b, t.matrix);
}

Matrix4::Matrix4()
{
    this->SetIdentity();
}

Matrix4::Matrix4(const Elements& elements)
{
    memcpy(this->matrix, elements, sizeof(float) * 16);
}

Matrix4::Matrix4(const Matrix4& a, const Matrix4& b)
{
    Matrix4::Multiply(a, b, this->matrix);
}

Matrix4::Matrix4(float t00, float t10, float t01, float t11, float x, float y)
{
    this->SetRawTransformation(t00, t10, t01, t11, x, y);
}

Matrix4::Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                 float ky)
{
    this->SetTransformation(x, y, angle, sx, sy, ox, oy, kx, ky);
}

bool Matrix4::IsAffine2DTransform() const
{
    return fabsf(this->matrix[2] + this->matrix[3] + this->matrix[6] + this->matrix[7] +
                 this->matrix[8] + this->matrix[9] + this->matrix[11] + this->matrix[14]) <
               0.00001f &&
           fabsf(this->matrix[10] + this->matrix[15] - 2.0f) < 0.00001f;
}

Matrix4 Matrix4::operator*(const Matrix4& m) const
{
    return Matrix4(*this, m);
}

void Matrix4::operator*=(const Matrix4& m)
{
    Elements matrix;
    Matrix4::Multiply(*this, m, matrix);
    memcpy(this->matrix, matrix, sizeof(float) * 16);
}

const Elements& Matrix4::GetElements() const
{
    return this->matrix;
}

void Matrix4::SetIdentity()
{
    memset(this->matrix, 0, sizeof(Elements));
    this->matrix[15] = this->matrix[10] = this->matrix[5] = this->matrix[0] = 1.0f;
}

void Matrix4::SetTranslation(float x, float y)
{
    this->SetIdentity();

    this->matrix[12] = x;
    this->matrix[13] = y;
}

void Matrix4::Translate(float x, float y)
{
    Matrix4 t;
    t.SetTranslation(x, y);
    this->operator*=(t);
}

void Matrix4::SetRotation(float rad)
{
    this->SetIdentity();
    float c = cosf(rad), s = sinf(rad);

    this->matrix[0] = c;
    this->matrix[4] = -s;
    this->matrix[1] = s;
    this->matrix[5] = c;
}

void Matrix4::Rotate(float rad)
{
    Matrix4 t;
    t.SetRotation(rad);
    this->operator*=(t);
}

void Matrix4::SetScale(float sx, float sy)
{
    this->SetIdentity();

    this->matrix[0] = sx;
    this->matrix[5] = sy;
}

void Matrix4::Scale(float sx, float sy)
{
    Matrix4 t;
    t.SetScale(sx, sy);
    this->operator*=(t);
}

void Matrix4::SetShear(float kx, float ky)
{
    this->SetIdentity();

    this->matrix[1] = ky;
    this->matrix[4] = kx;
}

void Matrix4::Shear(float kx, float ky)
{
    Matrix4 t;
    t.SetShear(kx, ky);
    this->operator*=(t);
}

void Matrix4::GetApproximateScale(float& sx, float& sy) const
{
    sx = sqrtf(this->matrix[0] * this->matrix[0] + this->matrix[4] * this->matrix[4]);
    sy = sqrtf(this->matrix[1] * this->matrix[1] + this->matrix[5] * this->matrix[5]);
}

void Matrix4::SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y)
{
    memset(this->matrix, 0, sizeof(float) * 16); // zero out matrix

    this->matrix[10] = this->matrix[15] = 1.0f;
    this->matrix[0]                     = t00;
    this->matrix[1]                     = t10;
    this->matrix[4]                     = t01;
    this->matrix[5]                     = t11;
    this->matrix[12]                    = x;
    this->matrix[13]                    = y;
}

void Matrix4::SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                                float oy, float kx, float ky)
{
    memset(this->matrix, 0, sizeof(float) * 16); // zero out matrix
    float c = cosf(angle), s = sinf(angle);

    this->matrix[10] = this->matrix[15] = 1.0f;
    this->matrix[0]                     = c * sx - ky * s * sy; // = a
    this->matrix[1]                     = s * sx + ky * c * sy; // = b
    this->matrix[4]                     = kx * c * sx - s * sy; // = c
    this->matrix[5]                     = kx * s * sx + c * sy; // = d
    this->matrix[12]                    = x - ox * this->matrix[0] - oy * this->matrix[4];
    this->matrix[13]                    = y - ox * this->matrix[1] - oy * this->matrix[5];
}

Matrix4 Matrix4::Inverse() const
{
    Matrix4 inv;

    inv.matrix[0] = this->matrix[5] * this->matrix[10] * this->matrix[15] -
                    this->matrix[5] * this->matrix[11] * this->matrix[14] -
                    this->matrix[9] * this->matrix[6] * this->matrix[15] +
                    this->matrix[9] * this->matrix[7] * this->matrix[14] +
                    this->matrix[13] * this->matrix[6] * this->matrix[11] -
                    this->matrix[13] * this->matrix[7] * this->matrix[10];

    inv.matrix[4] = -this->matrix[4] * this->matrix[10] * this->matrix[15] +
                    this->matrix[4] * this->matrix[11] * this->matrix[14] +
                    this->matrix[8] * this->matrix[6] * this->matrix[15] -
                    this->matrix[8] * this->matrix[7] * this->matrix[14] -
                    this->matrix[12] * this->matrix[6] * this->matrix[11] +
                    this->matrix[12] * this->matrix[7] * this->matrix[10];

    inv.matrix[8] = this->matrix[4] * this->matrix[9] * this->matrix[15] -
                    this->matrix[4] * this->matrix[11] * this->matrix[13] -
                    this->matrix[8] * this->matrix[5] * this->matrix[15] +
                    this->matrix[8] * this->matrix[7] * this->matrix[13] +
                    this->matrix[12] * this->matrix[5] * this->matrix[11] -
                    this->matrix[12] * this->matrix[7] * this->matrix[9];

    inv.matrix[12] = -this->matrix[4] * this->matrix[9] * this->matrix[14] +
                     this->matrix[4] * this->matrix[10] * this->matrix[13] +
                     this->matrix[8] * this->matrix[5] * this->matrix[14] -
                     this->matrix[8] * this->matrix[6] * this->matrix[13] -
                     this->matrix[12] * this->matrix[5] * this->matrix[10] +
                     this->matrix[12] * this->matrix[6] * this->matrix[9];

    inv.matrix[1] = -this->matrix[1] * this->matrix[10] * this->matrix[15] +
                    this->matrix[1] * this->matrix[11] * this->matrix[14] +
                    this->matrix[9] * this->matrix[2] * this->matrix[15] -
                    this->matrix[9] * this->matrix[3] * this->matrix[14] -
                    this->matrix[13] * this->matrix[2] * this->matrix[11] +
                    this->matrix[13] * this->matrix[3] * this->matrix[10];

    inv.matrix[5] = this->matrix[0] * this->matrix[10] * this->matrix[15] -
                    this->matrix[0] * this->matrix[11] * this->matrix[14] -
                    this->matrix[8] * this->matrix[2] * this->matrix[15] +
                    this->matrix[8] * this->matrix[3] * this->matrix[14] +
                    this->matrix[12] * this->matrix[2] * this->matrix[11] -
                    this->matrix[12] * this->matrix[3] * this->matrix[10];

    inv.matrix[9] = -this->matrix[0] * this->matrix[9] * this->matrix[15] +
                    this->matrix[0] * this->matrix[11] * this->matrix[13] +
                    this->matrix[8] * this->matrix[1] * this->matrix[15] -
                    this->matrix[8] * this->matrix[3] * this->matrix[13] -
                    this->matrix[12] * this->matrix[1] * this->matrix[11] +
                    this->matrix[12] * this->matrix[3] * this->matrix[9];

    inv.matrix[13] = this->matrix[0] * this->matrix[9] * this->matrix[14] -
                     this->matrix[0] * this->matrix[10] * this->matrix[13] -
                     this->matrix[8] * this->matrix[1] * this->matrix[14] +
                     this->matrix[8] * this->matrix[2] * this->matrix[13] +
                     this->matrix[12] * this->matrix[1] * this->matrix[10] -
                     this->matrix[12] * this->matrix[2] * this->matrix[9];

    inv.matrix[2] = this->matrix[1] * this->matrix[6] * this->matrix[15] -
                    this->matrix[1] * this->matrix[7] * this->matrix[14] -
                    this->matrix[5] * this->matrix[2] * this->matrix[15] +
                    this->matrix[5] * this->matrix[3] * this->matrix[14] +
                    this->matrix[13] * this->matrix[2] * this->matrix[7] -
                    this->matrix[13] * this->matrix[3] * this->matrix[6];

    inv.matrix[6] = -this->matrix[0] * this->matrix[6] * this->matrix[15] +
                    this->matrix[0] * this->matrix[7] * this->matrix[14] +
                    this->matrix[4] * this->matrix[2] * this->matrix[15] -
                    this->matrix[4] * this->matrix[3] * this->matrix[14] -
                    this->matrix[12] * this->matrix[2] * this->matrix[7] +
                    this->matrix[12] * this->matrix[3] * this->matrix[6];

    inv.matrix[10] = this->matrix[0] * this->matrix[5] * this->matrix[15] -
                     this->matrix[0] * this->matrix[7] * this->matrix[13] -
                     this->matrix[4] * this->matrix[1] * this->matrix[15] +
                     this->matrix[4] * this->matrix[3] * this->matrix[13] +
                     this->matrix[12] * this->matrix[1] * this->matrix[7] -
                     this->matrix[12] * this->matrix[3] * this->matrix[5];

    inv.matrix[14] = -this->matrix[0] * this->matrix[5] * this->matrix[14] +
                     this->matrix[0] * this->matrix[6] * this->matrix[13] +
                     this->matrix[4] * this->matrix[1] * this->matrix[14] -
                     this->matrix[4] * this->matrix[2] * this->matrix[13] -
                     this->matrix[12] * this->matrix[1] * this->matrix[6] +
                     this->matrix[12] * this->matrix[2] * this->matrix[5];

    inv.matrix[3] = -this->matrix[1] * this->matrix[6] * this->matrix[11] +
                    this->matrix[1] * this->matrix[7] * this->matrix[10] +
                    this->matrix[5] * this->matrix[2] * this->matrix[11] -
                    this->matrix[5] * this->matrix[3] * this->matrix[10] -
                    this->matrix[9] * this->matrix[2] * this->matrix[7] +
                    this->matrix[9] * this->matrix[3] * this->matrix[6];

    inv.matrix[7] = this->matrix[0] * this->matrix[6] * this->matrix[11] -
                    this->matrix[0] * this->matrix[7] * this->matrix[10] -
                    this->matrix[4] * this->matrix[2] * this->matrix[11] +
                    this->matrix[4] * this->matrix[3] * this->matrix[10] +
                    this->matrix[8] * this->matrix[2] * this->matrix[7] -
                    this->matrix[8] * this->matrix[3] * this->matrix[6];

    inv.matrix[11] = -this->matrix[0] * this->matrix[5] * this->matrix[11] +
                     this->matrix[0] * this->matrix[7] * this->matrix[9] +
                     this->matrix[4] * this->matrix[1] * this->matrix[11] -
                     this->matrix[4] * this->matrix[3] * this->matrix[9] -
                     this->matrix[8] * this->matrix[1] * this->matrix[7] +
                     this->matrix[8] * this->matrix[3] * this->matrix[5];

    inv.matrix[15] = this->matrix[0] * this->matrix[5] * this->matrix[10] -
                     this->matrix[0] * this->matrix[6] * this->matrix[9] -
                     this->matrix[4] * this->matrix[1] * this->matrix[10] +
                     this->matrix[4] * this->matrix[2] * this->matrix[9] +
                     this->matrix[8] * this->matrix[1] * this->matrix[6] -
                     this->matrix[8] * this->matrix[2] * this->matrix[5];

    float det = this->matrix[0] * inv.matrix[0] + this->matrix[1] * inv.matrix[4] +
                this->matrix[2] * inv.matrix[8] + this->matrix[3] * inv.matrix[12];

    float invdet = 1.0f / det;

    for (int i = 0; i < 16; i++)
        inv.matrix[i] *= invdet;

    return inv;
}

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float near, float far)
{
    Matrix4 m;

    m.matrix[0]  = 2.0f / (right - left);
    m.matrix[5]  = 2.0f / (top - bottom);
    m.matrix[10] = -2.0f / (far - near);

    m.matrix[12] = -(right + left) / (right - left);
    m.matrix[13] = -(top + bottom) / (top - bottom);
    m.matrix[14] = -(far + near) / (far - near);

    return m;
}