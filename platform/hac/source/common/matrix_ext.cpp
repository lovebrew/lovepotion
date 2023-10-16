#include <common/matrix_ext.hpp>

#include <arm_neon.h>
#include <string.h>

using namespace love;

void Matrix4<Console::HAC>::Multiply(const Matrix4& a, const Matrix4& b, float elements[16])
{
    float32x4_t cola1 = vld1q_f32(&a.elements[0]);
    float32x4_t cola2 = vld1q_f32(&a.elements[4]);
    float32x4_t cola3 = vld1q_f32(&a.elements[8]);
    float32x4_t cola4 = vld1q_f32(&a.elements[12]);

    float32x4_t col1 = vmulq_n_f32(cola1, b.elements[0]);
    col1             = vmlaq_n_f32(col1, cola2, b.elements[1]);
    col1             = vmlaq_n_f32(col1, cola3, b.elements[2]);
    col1             = vmlaq_n_f32(col1, cola4, b.elements[3]);

    float32x4_t col2 = vmulq_n_f32(cola1, b.elements[4]);
    col2             = vmlaq_n_f32(col2, cola2, b.elements[5]);
    col2             = vmlaq_n_f32(col2, cola3, b.elements[6]);
    col2             = vmlaq_n_f32(col2, cola4, b.elements[7]);

    float32x4_t col3 = vmulq_n_f32(cola1, b.elements[8]);
    col3             = vmlaq_n_f32(col3, cola2, b.elements[9]);
    col3             = vmlaq_n_f32(col3, cola3, b.elements[10]);
    col3             = vmlaq_n_f32(col3, cola4, b.elements[11]);

    float32x4_t col4 = vmulq_n_f32(cola1, b.elements[12]);
    col4             = vmlaq_n_f32(col4, cola2, b.elements[13]);
    col4             = vmlaq_n_f32(col4, cola3, b.elements[14]);
    col4             = vmlaq_n_f32(col4, cola4, b.elements[15]);

    vst1q_f32(&elements[0], col1);
    vst1q_f32(&elements[4], col2);
    vst1q_f32(&elements[8], col3);
    vst1q_f32(&elements[12], col4);
}

void Matrix4<Console::HAC>::Multiply(const Matrix4& a, const Matrix4& b, Matrix4& t)
{
    Matrix4<Console::HAC>::Multiply(a, b, t.elements);
}

Matrix4<Console::HAC>::Matrix4()
{
    this->SetIdentity();
}

Matrix4<Console::HAC>::Matrix4(const float elements[16])
{
    memcpy(this->elements, elements, sizeof(float) * 16);
}

Matrix4<Console::HAC>::Matrix4(const Matrix4& a, const Matrix4& b)
{
    Matrix4<Console::HAC>::Multiply(a, b, this->elements);
}

Matrix4<Console::HAC>::Matrix4(float t00, float t10, float t01, float t11, float x, float y)
{
    this->SetRawTransformation(t00, t10, t01, t11, x, y);
}

Matrix4<Console::HAC>::Matrix4(float x, float y, float angle, float sx, float sy, float ox,
                               float oy, float kx, float ky)
{
    this->SetTransformation(x, y, angle, sx, sy, ox, oy, kx, ky);
}

bool Matrix4<Console::HAC>::IsAffine2DTransform() const
{
    return fabsf(this->elements[2] + this->elements[3] + this->elements[6] + this->elements[7] +
                 this->elements[8] + this->elements[9] + this->elements[11] + this->elements[14]) <
               0.00001f &&
           fabsf(this->elements[10] + this->elements[15] - 2.0f) < 0.00001f;
}

Matrix4<Console::HAC> Matrix4<Console::HAC>::operator*(const Matrix4& m) const
{
    return Matrix4(*this, m);
}

void Matrix4<Console::HAC>::operator*=(const Matrix4& m)
{
    float elements[16];

    Matrix4<Console::HAC>::Multiply(*this, m, elements);
    memcpy(this->elements, elements, sizeof(float) * 16);
}

void Matrix4<Console::HAC>::SetIdentity()
{
    memset(this->elements, 0, sizeof(float) * 16);
    this->elements[15] = this->elements[10] = this->elements[5] = this->elements[0] = 1.0f;
}

void Matrix4<Console::HAC>::SetTranslation(float x, float y)
{
    this->SetIdentity();

    this->elements[12] = x;
    this->elements[13] = y;
}

void Matrix4<Console::HAC>::Translate(float x, float y)
{
    Matrix4 t;
    t.SetTranslation(x, y);
    this->operator*=(t);
}

void Matrix4<Console::HAC>::SetRotation(float rad)
{
    this->SetIdentity();
    float c = cosf(rad), s = sinf(rad);

    this->elements[0] = c;
    this->elements[4] = -s;
    this->elements[1] = s;
    this->elements[5] = c;
}

void Matrix4<Console::HAC>::Rotate(float rad)
{
    Matrix4 t;
    t.SetRotation(rad);
    this->operator*=(t);
}

void Matrix4<Console::HAC>::SetScale(float sx, float sy)
{
    this->SetIdentity();

    this->elements[0] = sx;
    this->elements[5] = sy;
}

void Matrix4<Console::HAC>::Scale(float sx, float sy)
{
    Matrix4 t;
    t.SetScale(sx, sy);
    this->operator*=(t);
}

void Matrix4<Console::HAC>::SetShear(float kx, float ky)
{
    this->SetIdentity();

    this->elements[1] = ky;
    this->elements[4] = kx;
}

void Matrix4<Console::HAC>::Shear(float kx, float ky)
{
    Matrix4 t;
    t.SetShear(kx, ky);
    this->operator*=(t);
}

void Matrix4<Console::HAC>::GetApproximateScale(float& sx, float& sy) const
{
    sx = sqrtf(this->elements[0] * this->elements[0] + this->elements[4] * this->elements[4]);
    sy = sqrtf(this->elements[1] * this->elements[1] + this->elements[5] * this->elements[5]);
}

void Matrix4<Console::HAC>::SetRawTransformation(float t00, float t10, float t01, float t11,
                                                 float x, float y)
{
    memset(this->elements, 0, sizeof(float) * 16); // zero out matrix

    this->elements[10] = this->elements[15] = 1.0f;
    this->elements[0]                       = t00;
    this->elements[1]                       = t10;
    this->elements[4]                       = t01;
    this->elements[5]                       = t11;
    this->elements[12]                      = x;
    this->elements[13]                      = y;
}

void Matrix4<Console::HAC>::SetTransformation(float x, float y, float angle, float sx, float sy,
                                              float ox, float oy, float kx, float ky)
{
    memset(this->elements, 0, sizeof(float) * 16); // zero out matrix
    float c = cosf(angle), s = sinf(angle);

    this->elements[10] = this->elements[15] = 1.0f;
    this->elements[0]                       = c * sx - ky * s * sy; // = a
    this->elements[1]                       = s * sx + ky * c * sy; // = b
    this->elements[4]                       = kx * c * sx - s * sy; // = c
    this->elements[5]                       = kx * s * sx + c * sy; // = d
    this->elements[12]                      = x - ox * this->elements[0] - oy * this->elements[4];
    this->elements[13]                      = y - ox * this->elements[1] - oy * this->elements[5];
}

Matrix4<Console::HAC> Matrix4<Console::HAC>::Inverse() const
{
    Matrix4<Console::HAC> inv;

    inv.elements[0] = this->elements[5] * this->elements[10] * this->elements[15] -
                      this->elements[5] * this->elements[11] * this->elements[14] -
                      this->elements[9] * this->elements[6] * this->elements[15] +
                      this->elements[9] * this->elements[7] * this->elements[14] +
                      this->elements[13] * this->elements[6] * this->elements[11] -
                      this->elements[13] * this->elements[7] * this->elements[10];

    inv.elements[4] = -this->elements[4] * this->elements[10] * this->elements[15] +
                      this->elements[4] * this->elements[11] * this->elements[14] +
                      this->elements[8] * this->elements[6] * this->elements[15] -
                      this->elements[8] * this->elements[7] * this->elements[14] -
                      this->elements[12] * this->elements[6] * this->elements[11] +
                      this->elements[12] * this->elements[7] * this->elements[10];

    inv.elements[8] = this->elements[4] * this->elements[9] * this->elements[15] -
                      this->elements[4] * this->elements[11] * this->elements[13] -
                      this->elements[8] * this->elements[5] * this->elements[15] +
                      this->elements[8] * this->elements[7] * this->elements[13] +
                      this->elements[12] * this->elements[5] * this->elements[11] -
                      this->elements[12] * this->elements[7] * this->elements[9];

    inv.elements[12] = -this->elements[4] * this->elements[9] * this->elements[14] +
                       this->elements[4] * this->elements[10] * this->elements[13] +
                       this->elements[8] * this->elements[5] * this->elements[14] -
                       this->elements[8] * this->elements[6] * this->elements[13] -
                       this->elements[12] * this->elements[5] * this->elements[10] +
                       this->elements[12] * this->elements[6] * this->elements[9];

    inv.elements[1] = -this->elements[1] * this->elements[10] * this->elements[15] +
                      this->elements[1] * this->elements[11] * this->elements[14] +
                      this->elements[9] * this->elements[2] * this->elements[15] -
                      this->elements[9] * this->elements[3] * this->elements[14] -
                      this->elements[13] * this->elements[2] * this->elements[11] +
                      this->elements[13] * this->elements[3] * this->elements[10];

    inv.elements[5] = this->elements[0] * this->elements[10] * this->elements[15] -
                      this->elements[0] * this->elements[11] * this->elements[14] -
                      this->elements[8] * this->elements[2] * this->elements[15] +
                      this->elements[8] * this->elements[3] * this->elements[14] +
                      this->elements[12] * this->elements[2] * this->elements[11] -
                      this->elements[12] * this->elements[3] * this->elements[10];

    inv.elements[9] = -this->elements[0] * this->elements[9] * this->elements[15] +
                      this->elements[0] * this->elements[11] * this->elements[13] +
                      this->elements[8] * this->elements[1] * this->elements[15] -
                      this->elements[8] * this->elements[3] * this->elements[13] -
                      this->elements[12] * this->elements[1] * this->elements[11] +
                      this->elements[12] * this->elements[3] * this->elements[9];

    inv.elements[13] = this->elements[0] * this->elements[9] * this->elements[14] -
                       this->elements[0] * this->elements[10] * this->elements[13] -
                       this->elements[8] * this->elements[1] * this->elements[14] +
                       this->elements[8] * this->elements[2] * this->elements[13] +
                       this->elements[12] * this->elements[1] * this->elements[10] -
                       this->elements[12] * this->elements[2] * this->elements[9];

    inv.elements[2] = this->elements[1] * this->elements[6] * this->elements[15] -
                      this->elements[1] * this->elements[7] * this->elements[14] -
                      this->elements[5] * this->elements[2] * this->elements[15] +
                      this->elements[5] * this->elements[3] * this->elements[14] +
                      this->elements[13] * this->elements[2] * this->elements[7] -
                      this->elements[13] * this->elements[3] * this->elements[6];

    inv.elements[6] = -this->elements[0] * this->elements[6] * this->elements[15] +
                      this->elements[0] * this->elements[7] * this->elements[14] +
                      this->elements[4] * this->elements[2] * this->elements[15] -
                      this->elements[4] * this->elements[3] * this->elements[14] -
                      this->elements[12] * this->elements[2] * this->elements[7] +
                      this->elements[12] * this->elements[3] * this->elements[6];

    inv.elements[10] = this->elements[0] * this->elements[5] * this->elements[15] -
                       this->elements[0] * this->elements[7] * this->elements[13] -
                       this->elements[4] * this->elements[1] * this->elements[15] +
                       this->elements[4] * this->elements[3] * this->elements[13] +
                       this->elements[12] * this->elements[1] * this->elements[7] -
                       this->elements[12] * this->elements[3] * this->elements[5];

    inv.elements[14] = -this->elements[0] * this->elements[5] * this->elements[14] +
                       this->elements[0] * this->elements[6] * this->elements[13] +
                       this->elements[4] * this->elements[1] * this->elements[14] -
                       this->elements[4] * this->elements[2] * this->elements[13] -
                       this->elements[12] * this->elements[1] * this->elements[6] +
                       this->elements[12] * this->elements[2] * this->elements[5];

    inv.elements[3] = -this->elements[1] * this->elements[6] * this->elements[11] +
                      this->elements[1] * this->elements[7] * this->elements[10] +
                      this->elements[5] * this->elements[2] * this->elements[11] -
                      this->elements[5] * this->elements[3] * this->elements[10] -
                      this->elements[9] * this->elements[2] * this->elements[7] +
                      this->elements[9] * this->elements[3] * this->elements[6];

    inv.elements[7] = this->elements[0] * this->elements[6] * this->elements[11] -
                      this->elements[0] * this->elements[7] * this->elements[10] -
                      this->elements[4] * this->elements[2] * this->elements[11] +
                      this->elements[4] * this->elements[3] * this->elements[10] +
                      this->elements[8] * this->elements[2] * this->elements[7] -
                      this->elements[8] * this->elements[3] * this->elements[6];

    inv.elements[11] = -this->elements[0] * this->elements[5] * this->elements[11] +
                       this->elements[0] * this->elements[7] * this->elements[9] +
                       this->elements[4] * this->elements[1] * this->elements[11] -
                       this->elements[4] * this->elements[3] * this->elements[9] -
                       this->elements[8] * this->elements[1] * this->elements[7] +
                       this->elements[8] * this->elements[3] * this->elements[5];

    inv.elements[15] = this->elements[0] * this->elements[5] * this->elements[10] -
                       this->elements[0] * this->elements[6] * this->elements[9] -
                       this->elements[4] * this->elements[1] * this->elements[10] +
                       this->elements[4] * this->elements[2] * this->elements[9] +
                       this->elements[8] * this->elements[1] * this->elements[6] -
                       this->elements[8] * this->elements[2] * this->elements[5];

    float det = this->elements[0] * inv.elements[0] + this->elements[1] * inv.elements[4] +
                this->elements[2] * inv.elements[8] + this->elements[3] * inv.elements[12];

    float invdet = 1.0f / det;

    for (int i = 0; i < 16; i++)
        inv.elements[i] *= invdet;

    return inv;
}

Matrix4<Console::HAC> Matrix4<Console::HAC>::Ortho(float left, float right, float bottom, float top,
                                                   float near, float far)
{
    Matrix4<Console::HAC> matrix;

    matrix.elements[0]  = 2.0f / (right - left);
    matrix.elements[5]  = 2.0f / (top - bottom);
    matrix.elements[10] = -2.0f / (far - near);

    matrix.elements[12] = -(right + left) / (right - left);
    matrix.elements[13] = -(top + bottom) / (top - bottom);
    matrix.elements[14] = -(far + near) / (far - near);

    return matrix;
}
