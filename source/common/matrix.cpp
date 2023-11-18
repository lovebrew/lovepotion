#include <common/matrix.hpp>

using namespace love;

#if defined(__SWITCH__)
    #include <arm_neon.h>
#endif

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, float t[0x10])
{
#if defined(__SWITCH__)
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
#else
    // clang-format off
    t[0]  = (a.matrix[0] * b.matrix[0])  + (a.matrix[4] * b.matrix[1])  + (a.matrix[8] * b.matrix[2])  + (a.matrix[12] * b.matrix[3]);
    t[4]  = (a.matrix[0] * b.matrix[4])  + (a.matrix[4] * b.matrix[5])  + (a.matrix[8] * b.matrix[6])  + (a.matrix[12] * b.matrix[7]);
    t[8]  = (a.matrix[0] * b.matrix[8])  + (a.matrix[4] * b.matrix[9])  + (a.matrix[8] * b.matrix[10]) + (a.matrix[12] * b.matrix[11]);
    t[12] = (a.matrix[0] * b.matrix[12]) + (a.matrix[4] * b.matrix[13]) + (a.matrix[8] * b.matrix[14]) + (a.matrix[12] * b.matrix[15]);

    t[1]  = (a.matrix[1] * b.matrix[0])  + (a.matrix[5] * b.matrix[1])  + (a.matrix[9] * b.matrix[2])  + (a.matrix[13] * b.matrix[3]);
    t[5]  = (a.matrix[1] * b.matrix[4])  + (a.matrix[5] * b.matrix[5])  + (a.matrix[9] * b.matrix[6])  + (a.matrix[13] * b.matrix[7]);
    t[9]  = (a.matrix[1] * b.matrix[8])  + (a.matrix[5] * b.matrix[9])  + (a.matrix[9] * b.matrix[10]) + (a.matrix[13] * b.matrix[11]);
    t[13] = (a.matrix[1] * b.matrix[12]) + (a.matrix[5] * b.matrix[13]) + (a.matrix[9] * b.matrix[14]) + (a.matrix[13] * b.matrix[15]);

    t[2]  = (a.matrix[2] * b.matrix[0])  + (a.matrix[6] * b.matrix[1])  + (a.matrix[10] * b.matrix[2])  + (a.matrix[14] * b.matrix[3]);
    t[6]  = (a.matrix[2] * b.matrix[4])  + (a.matrix[6] * b.matrix[5])  + (a.matrix[10] * b.matrix[6])  + (a.matrix[14] * b.matrix[7]);
    t[10] = (a.matrix[2] * b.matrix[8])  + (a.matrix[6] * b.matrix[9])  + (a.matrix[10] * b.matrix[10]) + (a.matrix[14] * b.matrix[11]);
    t[14] = (a.matrix[2] * b.matrix[12]) + (a.matrix[6] * b.matrix[13]) + (a.matrix[10] * b.matrix[14]) + (a.matrix[14] * b.matrix[15]);

    t[3]  = (a.matrix[3] * b.matrix[0])  + (a.matrix[7] * b.matrix[1])  + (a.matrix[11] * b.matrix[2])  + (a.matrix[15] * b.matrix[3]);
    t[7]  = (a.matrix[3] * b.matrix[4])  + (a.matrix[7] * b.matrix[5])  + (a.matrix[11] * b.matrix[6])  + (a.matrix[15] * b.matrix[7]);
    t[11] = (a.matrix[3] * b.matrix[8])  + (a.matrix[7] * b.matrix[9])  + (a.matrix[11] * b.matrix[10]) + (a.matrix[15] * b.matrix[11]);
    t[15] = (a.matrix[3] * b.matrix[12]) + (a.matrix[7] * b.matrix[13]) + (a.matrix[11] * b.matrix[14]) + (a.matrix[15] * b.matrix[15]);
    // clang-format on
#endif
}

void Matrix4::Multiply(const Matrix4& a, const Matrix4& b, Matrix4& dst)
{
    Multiply(a, b, dst.matrix);
}

Matrix4::Matrix4()
{
    this->SetIdentity();
}

Matrix4::Matrix4(const float matrix[0x10])
{
    std::copy_n(matrix, 0x10, this->matrix);
}

Matrix4::Matrix4(float t00, float t10, float t01, float t11, float x, float y)
{
    this->SetRawTransformation(t00, t10, t01, t11, x, y);
}

Matrix4::Matrix4(const Matrix4& a, const Matrix4& b)
{
    Matrix4::Multiply(a, b, this->matrix);
}

Matrix4::Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                 float ky)
{
    this->SetTransformation(x, y, angle, sx, sy, ox, oy, kx, ky);
}

Matrix4 Matrix4::operator*(const Matrix4& other) const
{
    return Matrix4(*this, other);
}

void Matrix4::operator*=(const Matrix4& other)
{
    float temp[0x10] { 0 };
    Matrix4::Multiply(*this, other, temp);
    std::copy_n(temp, 0x10, this->matrix);
}

void Matrix4::SetRow(int row, const Vector4& vector)
{
    this->matrix[0 * 4 + row] = vector.x;
    this->matrix[1 * 4 + row] = vector.y;
    this->matrix[2 * 4 + row] = vector.z;
    this->matrix[3 * 4 + row] = vector.w;
}

Vector4 Matrix4::GetRow(int row)
{
    return Vector4(this->matrix[0 * 4 + row], this->matrix[1 * 4 + row], this->matrix[2 * 4 + row],
                   this->matrix[3 * 4 + row]);
}

void Matrix4::SetColumn(int column, const Vector4& vector)
{
    this->matrix[column * 4 + 0] = vector.x;
    this->matrix[column * 4 + 1] = vector.y;
    this->matrix[column * 4 + 2] = vector.z;
    this->matrix[column * 4 + 3] = vector.w;
}

Vector4 Matrix4::GetColumn(int column)
{
    return Vector4(this->matrix[column * 4 + 0], this->matrix[column * 4 + 1],
                   this->matrix[column * 4 + 2], this->matrix[column * 4 + 3]);
}

void Matrix4::SetIdentity()
{
    std::fill_n(this->matrix, 0x10, 0.0f);
    this->matrix[15] = this->matrix[10] = this->matrix[5] = this->matrix[0] = 1;
}

void Matrix4::SetTranslation(float x, float y)
{
    this->SetIdentity();
    this->matrix[12] = x;
    this->matrix[13] = y;
}

void Matrix4::SetRotation(float rotation)
{
    this->SetIdentity();
    float c = std::cos(rotation);
    float s = std::sin(rotation);

    this->matrix[0] = c;
    this->matrix[1] = s;
    this->matrix[4] = -s;
    this->matrix[5] = c;
}

void Matrix4::SetScale(float sx, float sy)
{
    this->SetIdentity();

    this->matrix[0] = sx;
    this->matrix[5] = sy;
}

void Matrix4::SetShear(float kx, float ky)
{
    this->SetIdentity();

    this->matrix[1] = ky;
    this->matrix[4] = kx;
}

void Matrix4::GetApproximateScale(float& sx, float& sy) const
{
    sx = std::sqrt(this->matrix[0] * this->matrix[0] + this->matrix[4] * this->matrix[4]);
    sy = std::sqrt(this->matrix[1] * this->matrix[1] + this->matrix[5] * this->matrix[5]);
}

void Matrix4::SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y)
{
    std::fill_n(this->matrix, 0x10, 0.0f);
    this->matrix[10] = this->matrix[15] = 1.0f;

    this->matrix[0]  = t00;
    this->matrix[1]  = t10;
    this->matrix[4]  = t01;
    this->matrix[5]  = t11;
    this->matrix[12] = x;
    this->matrix[13] = y;
}

void Matrix4::SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                                float oy, float kx, float ky)
{
    std::fill_n(this->matrix, 0x10, 0.0f);

    float c = std::cos(angle);
    float s = std::sin(angle);

    this->matrix[10] = this->matrix[15] = 1.0f;

    this->matrix[0]  = c * sx - ky * s * sy; // = a
    this->matrix[1]  = s * sx + ky * c * sy; // = b
    this->matrix[4]  = kx * c * sx - s * sy; // = c
    this->matrix[5]  = kx * s * sx + c * sy; // = d
    this->matrix[12] = x - ox * this->matrix[0] - oy * this->matrix[4];
    this->matrix[13] = y - ox * this->matrix[1] - oy * this->matrix[5];
}

void Matrix4::Translate(float x, float y)
{
    Matrix4 temp {};
    temp.SetTranslation(x, y);
    this->operator*=(temp);
}

void Matrix4::Rotate(float rotation)
{
    Matrix4 temp {};
    temp.SetRotation(rotation);
    this->operator*=(temp);
}

void Matrix4::Scale(float sx, float sy)
{
    Matrix4 temp {};
    temp.SetScale(sx, sy);
    this->operator*=(temp);
}

void Matrix4::Shear(float kx, float ky)
{
    Matrix4 temp {};
    temp.SetShear(kx, ky);
    this->operator*=(temp);
}

bool Matrix4::IsAffine2DTransform() const
{
    return fabsf(this->matrix[2] + this->matrix[3] + this->matrix[6] + this->matrix[7] +
                 this->matrix[8] + this->matrix[9] + this->matrix[11] + this->matrix[14]) <
               0.00001f &&
           fabsf(this->matrix[10] + this->matrix[15] - 2.0f) < 0.00001f;
}

Matrix4 Matrix4::Inverse() const
{
    Matrix4 inverse {};

    inverse.matrix[0] = this->matrix[5] * this->matrix[10] * this->matrix[15] -
                        this->matrix[5] * this->matrix[11] * this->matrix[14] -
                        this->matrix[9] * this->matrix[6] * this->matrix[15] +
                        this->matrix[9] * this->matrix[7] * this->matrix[14] +
                        this->matrix[13] * this->matrix[6] * this->matrix[11] -
                        this->matrix[13] * this->matrix[7] * this->matrix[10];

    inverse.matrix[4] = -this->matrix[4] * this->matrix[10] * this->matrix[15] +
                        this->matrix[4] * this->matrix[11] * this->matrix[14] +
                        this->matrix[8] * this->matrix[6] * this->matrix[15] -
                        this->matrix[8] * this->matrix[7] * this->matrix[14] -
                        this->matrix[12] * this->matrix[6] * this->matrix[11] +
                        this->matrix[12] * this->matrix[7] * this->matrix[10];

    inverse.matrix[8] = this->matrix[4] * this->matrix[9] * this->matrix[15] -
                        this->matrix[4] * this->matrix[11] * this->matrix[13] -
                        this->matrix[8] * this->matrix[5] * this->matrix[15] +
                        this->matrix[8] * this->matrix[7] * this->matrix[13] +
                        this->matrix[12] * this->matrix[5] * this->matrix[11] -
                        this->matrix[12] * this->matrix[7] * this->matrix[9];

    inverse.matrix[12] = -this->matrix[4] * this->matrix[9] * this->matrix[14] +
                         this->matrix[4] * this->matrix[10] * this->matrix[13] +
                         this->matrix[8] * this->matrix[5] * this->matrix[14] -
                         this->matrix[8] * this->matrix[6] * this->matrix[13] -
                         this->matrix[12] * this->matrix[5] * this->matrix[10] +
                         this->matrix[12] * this->matrix[6] * this->matrix[9];

    inverse.matrix[1] = -this->matrix[1] * this->matrix[10] * this->matrix[15] +
                        this->matrix[1] * this->matrix[11] * this->matrix[14] +
                        this->matrix[9] * this->matrix[2] * this->matrix[15] -
                        this->matrix[9] * this->matrix[3] * this->matrix[14] -
                        this->matrix[13] * this->matrix[2] * this->matrix[11] +
                        this->matrix[13] * this->matrix[3] * this->matrix[10];

    inverse.matrix[5] = this->matrix[0] * this->matrix[10] * this->matrix[15] -
                        this->matrix[0] * this->matrix[11] * this->matrix[14] -
                        this->matrix[8] * this->matrix[2] * this->matrix[15] +
                        this->matrix[8] * this->matrix[3] * this->matrix[14] +
                        this->matrix[12] * this->matrix[2] * this->matrix[11] -
                        this->matrix[12] * this->matrix[3] * this->matrix[10];

    inverse.matrix[9] = -this->matrix[0] * this->matrix[9] * this->matrix[15] +
                        this->matrix[0] * this->matrix[11] * this->matrix[13] +
                        this->matrix[8] * this->matrix[1] * this->matrix[15] -
                        this->matrix[8] * this->matrix[3] * this->matrix[13] -
                        this->matrix[12] * this->matrix[1] * this->matrix[11] +
                        this->matrix[12] * this->matrix[3] * this->matrix[9];

    inverse.matrix[13] = this->matrix[0] * this->matrix[9] * this->matrix[14] -
                         this->matrix[0] * this->matrix[10] * this->matrix[13] -
                         this->matrix[8] * this->matrix[1] * this->matrix[14] +
                         this->matrix[8] * this->matrix[2] * this->matrix[13] +
                         this->matrix[12] * this->matrix[1] * this->matrix[10] -
                         this->matrix[12] * this->matrix[2] * this->matrix[9];

    inverse.matrix[2] = this->matrix[1] * this->matrix[6] * this->matrix[15] -
                        this->matrix[1] * this->matrix[7] * this->matrix[14] -
                        this->matrix[5] * this->matrix[2] * this->matrix[15] +
                        this->matrix[5] * this->matrix[3] * this->matrix[14] +
                        this->matrix[13] * this->matrix[2] * this->matrix[7] -
                        this->matrix[13] * this->matrix[3] * this->matrix[6];

    inverse.matrix[6] = -this->matrix[0] * this->matrix[6] * this->matrix[15] +
                        this->matrix[0] * this->matrix[7] * this->matrix[14] +
                        this->matrix[4] * this->matrix[2] * this->matrix[15] -
                        this->matrix[4] * this->matrix[3] * this->matrix[14] -
                        this->matrix[12] * this->matrix[2] * this->matrix[7] +
                        this->matrix[12] * this->matrix[3] * this->matrix[6];

    inverse.matrix[10] = this->matrix[0] * this->matrix[5] * this->matrix[15] -
                         this->matrix[0] * this->matrix[7] * this->matrix[13] -
                         this->matrix[4] * this->matrix[1] * this->matrix[15] +
                         this->matrix[4] * this->matrix[3] * this->matrix[13] +
                         this->matrix[12] * this->matrix[1] * this->matrix[7] -
                         this->matrix[12] * this->matrix[3] * this->matrix[5];

    inverse.matrix[14] = -this->matrix[0] * this->matrix[5] * this->matrix[14] +
                         this->matrix[0] * this->matrix[6] * this->matrix[13] +
                         this->matrix[4] * this->matrix[1] * this->matrix[14] -
                         this->matrix[4] * this->matrix[2] * this->matrix[13] -
                         this->matrix[12] * this->matrix[1] * this->matrix[6] +
                         this->matrix[12] * this->matrix[2] * this->matrix[5];

    inverse.matrix[3] = -this->matrix[1] * this->matrix[6] * this->matrix[11] +
                        this->matrix[1] * this->matrix[7] * this->matrix[10] +
                        this->matrix[5] * this->matrix[2] * this->matrix[11] -
                        this->matrix[5] * this->matrix[3] * this->matrix[10] -
                        this->matrix[9] * this->matrix[2] * this->matrix[7] +
                        this->matrix[9] * this->matrix[3] * this->matrix[6];

    inverse.matrix[7] = this->matrix[0] * this->matrix[6] * this->matrix[11] -
                        this->matrix[0] * this->matrix[7] * this->matrix[10] -
                        this->matrix[4] * this->matrix[2] * this->matrix[11] +
                        this->matrix[4] * this->matrix[3] * this->matrix[10] +
                        this->matrix[8] * this->matrix[2] * this->matrix[7] -
                        this->matrix[8] * this->matrix[3] * this->matrix[6];

    inverse.matrix[11] = -this->matrix[0] * this->matrix[5] * this->matrix[11] +
                         this->matrix[0] * this->matrix[7] * this->matrix[9] +
                         this->matrix[4] * this->matrix[1] * this->matrix[11] -
                         this->matrix[4] * this->matrix[3] * this->matrix[9] -
                         this->matrix[8] * this->matrix[1] * this->matrix[7] +
                         this->matrix[8] * this->matrix[3] * this->matrix[5];

    inverse.matrix[15] = this->matrix[0] * this->matrix[5] * this->matrix[10] -
                         this->matrix[0] * this->matrix[6] * this->matrix[9] -
                         this->matrix[4] * this->matrix[1] * this->matrix[10] +
                         this->matrix[4] * this->matrix[2] * this->matrix[9] +
                         this->matrix[8] * this->matrix[1] * this->matrix[6] -
                         this->matrix[8] * this->matrix[2] * this->matrix[5];

    float det = this->matrix[0] * inverse.matrix[0] + this->matrix[1] * inverse.matrix[4] +
                this->matrix[2] * inverse.matrix[8] + this->matrix[3] * inverse.matrix[12];

    float invDet = 1.0f / det;

    for (int i = 0; i < 16; i++)
        inverse.matrix[i] *= invDet;

    return inverse;
}

Matrix4 Matrix4::Ortho(float left, float right, float bottom, float top, float near, float far)
{
    Matrix4 result {};

    result.matrix[0]  = 2.0f / (right - left);
    result.matrix[5]  = 2.0f / (top - bottom);
    result.matrix[10] = -2.0f / (far - near);

    result.matrix[12] = -(right + left) / (right - left);
    result.matrix[13] = -(top + bottom) / (top - bottom);
    result.matrix[14] = -(far + near) / (far - near);

    return result;
}

Matrix4 Matrix4::Perspective(float verticalfov, float aspect, float near, float far)
{
    Matrix4 result {};

    float coTangent = 1.0f / std::tan(verticalfov * 0.5f);

    result.matrix[0]  = coTangent / aspect;
    result.matrix[5]  = coTangent;
    result.matrix[10] = (near + far) / (near - far);
    result.matrix[11] = -1.0f;

    result.matrix[14] = (2.0f * near * far) / (near - far);
    result.matrix[15] = 0.0f;

    return result;
}
