#pragma once

#include "console.hpp"
#include "math.hpp"

#include <cstring>

#if defined(__3DS__)
    #include <citro3d.h>
using Elements = C3D_Mtx;
#else
using Elements = float[0x10];
#endif

#if defined(__SWITCH__)
    #include <arm_neon.h>
#endif

#include <span>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Matrix4
    {
      protected:
        static void Multiply(const Matrix4& a, const Matrix4& b, Elements t)
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
            matrix[0] = (a.matrix[0] * b.matrix[0]) + (a.matrix[4] * b.matrix[1]) +
                        (a.matrix[8] * b.matrix[2]) + (a.matrix[12] * b.matrix[3]);
            matrix[4] = (a.matrix[0] * b.matrix[4]) + (a.matrix[4] * b.matrix[5]) +
                        (a.matrix[8] * b.matrix[6]) + (a.matrix[12] * b.matrix[7]);
            matrix[8] = (a.matrix[0] * b.matrix[8]) + (a.matrix[4] * b.matrix[9]) +
                        (a.matrix[8] * b.matrix[10]) + (a.matrix[12] * b.matrix[11]);
            matrix[12] = (a.matrix[0] * b.matrix[12]) + (a.matrix[4] * b.matrix[13]) +
                         (a.matrix[8] * b.matrix[14]) + (a.matrix[12] * b.matrix[15]);

            matrix[1] = (a.matrix[1] * b.matrix[0]) + (a.matrix[5] * b.matrix[1]) +
                        (a.matrix[9] * b.matrix[2]) + (a.matrix[13] * b.matrix[3]);
            matrix[5] = (a.matrix[1] * b.matrix[4]) + (a.matrix[5] * b.matrix[5]) +
                        (a.matrix[9] * b.matrix[6]) + (a.matrix[13] * b.matrix[7]);
            matrix[9] = (a.matrix[1] * b.matrix[8]) + (a.matrix[5] * b.matrix[9]) +
                        (a.matrix[9] * b.matrix[10]) + (a.matrix[13] * b.matrix[11]);
            matrix[13] = (a.matrix[1] * b.matrix[12]) + (a.matrix[5] * b.matrix[13]) +
                         (a.matrix[9] * b.matrix[14]) + (a.matrix[13] * b.matrix[15]);

            matrix[2] = (a.matrix[2] * b.matrix[0]) + (a.matrix[6] * b.matrix[1]) +
                        (a.matrix[10] * b.matrix[2]) + (a.matrix[14] * b.matrix[3]);
            matrix[6] = (a.matrix[2] * b.matrix[4]) + (a.matrix[6] * b.matrix[5]) +
                        (a.matrix[10] * b.matrix[6]) + (a.matrix[14] * b.matrix[7]);
            matrix[10] = (a.matrix[2] * b.matrix[8]) + (a.matrix[6] * b.matrix[9]) +
                         (a.matrix[10] * b.matrix[10]) + (a.matrix[14] * b.matrix[11]);
            matrix[14] = (a.matrix[2] * b.matrix[12]) + (a.matrix[6] * b.matrix[13]) +
                         (a.matrix[10] * b.matrix[14]) + (a.matrix[14] * b.matrix[15]);

            matrix[3] = (a.matrix[3] * b.matrix[0]) + (a.matrix[7] * b.matrix[1]) +
                        (a.matrix[11] * b.matrix[2]) + (a.matrix[15] * b.matrix[3]);
            matrix[7] = (a.matrix[3] * b.matrix[4]) + (a.matrix[7] * b.matrix[5]) +
                        (a.matrix[11] * b.matrix[6]) + (a.matrix[15] * b.matrix[7]);
            matrix[11] = (a.matrix[3] * b.matrix[8]) + (a.matrix[7] * b.matrix[9]) +
                         (a.matrix[11] * b.matrix[10]) + (a.matrix[15] * b.matrix[11]);
            matrix[15] = (a.matrix[3] * b.matrix[12]) + (a.matrix[7] * b.matrix[13]) +
                         (a.matrix[11] * b.matrix[14]) + (a.matrix[15] * b.matrix[15]);
#endif
        }

      public:
        static void Multiply(const Matrix4& a, const Matrix4& b, Matrix4& result)
        {
            Multiply(a, b, result.matrix);
        }

        Matrix4()
        {
            this->SetIdentity();
        }

        Matrix4(float t00, float t10, float t01, float t11, float x, float y)
        {
            this->SetRawTransformation(float t00, float t10, float t01, float t11, float x,
                                       float y);
        }

        Matrix4(const Elements elements)
        {
            std::copy_n(elements, 16, this->matrix);
        }

        Matrix4(const Matrix4& a, const Matrix4& b)
        {
            this->Multiply(a, b);
        }

        Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                float ky)
        {
            this->SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                                    float oy, float kx, float ky);
        }

        Matrix4 operator*(const Matrix4& other) const
        {
            return Matrix4(*this, other);
        }

        void operator*=(const Matrix4& other)
        {
            Elements elements;
            this->Multiply(*this, other, elements);
            std::copy_n(elements, 16, this->matrix);
        }

        const Elements* GetElements() const
        {
            return this->matrix;
        }

        void SetRow(int row, const Vector4& vector)
        {
            this->matrix[0 * 4 + r] = v.x;
            this->matrix[1 * 4 + r] = v.y;
            this->matrix[2 * 4 + r] = v.z;
            this->matrix[3 * 4 + r] = v.w;
        }

        Vector4 GetRow(int row)
        {
            return Vector4(this->matrix[0 * 4 + r], this->matrix[1 * 4 + r],
                           this->matrix[2 * 4 + r], this->matrix[3 * 4 + r]);
        }

        void SetColumn(int column, const Vector4& vector)
        {
            this->matrix[c * 4 + 0] = v.x;
            this->matrix[c * 4 + 1] = v.y;
            this->matrix[c * 4 + 2] = v.z;
            this->matrix[c * 4 + 3] = v.w;
        }

        Vector4 GetColumn(int column)
        {
            return Vector4(this->matrix[c * 4 + 0], this->matrix[c * 4 + 1],
                           this->matrix[c * 4 + 2], this->matrix[c * 4 + 3]);
        }

        void SetIdentity()
        {
            std::fill_n(this->matrix, 16, 0.0f);
            this->matrix[15] = this->matrix[10] = this->matrix[5] = this->matrix[0] = 1;
        }

        void SetTranslation(float x, float y)
        {
            this->SetIdentity();
            this->matrix[12] = x;
            this->matrix[13] = y;
        }

        void SetRotation(float rotation)
        {
            this->SetIdentity();

            float cos = std::cos(rotation);
            float sin = std::sin(rotation);

            this->matrix[0] = cos;
            this->matrix[4] = -sin;
            this->matrix[1] = sin;
            this->matrix[5] = cos;
        }

        void SetScale(float sx, float sy)
        {
            this->SetIdentity();

            this->matrix[0] = sx;
            this->matrix[5] = sy;
        }

        void SetShear(float kx, float ky)
        {
            this->SetIdentity();

            this->matrix[1] = ky;
            this->matrix[4] = kx;
        }

        void GetApproximateScale(float& sx, float& sy) const
        {
            sx = std::sqrt(this->matrix[0] * this->matrix[0] + this->matrix[4] * this->matrix[4]);
            sy = std::sqrt(this->matrix[1] * this->matrix[1] + this->matrix[5] * this->matrix[5]);
        }

        void SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y)
        {
            std::fill_n(this->matrix, 16, 0.0f);

            this->matrix[10] = this->matrix[15] = 1.0f;
            this->matrix[0]                     = t00;
            this->matrix[1]                     = t10;
            this->matrix[4]                     = t01;
            this->matrix[5]                     = t11;
            this->matrix[12]                    = x;
            this->matrix[13]                    = y;
        }

        void SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                               float oy, float kx, float ky)
        {
            std::fill_n(this->matrix, 16, 0.0f);

            float cos = std::cos(angle);
            float sin = std::sin(angle);

            this->matrix[10] = this->matrix[15] = 1.0f;
            this->matrix[0]                     = cos * sx - ky * sin * sy; // = a
            this->matrix[1]                     = sin * sx + ky * cos * sy; // = b
            this->matrix[4]                     = kx * cos * sx - sin * sy; // = c
            this->matrix[5]                     = kx * sin * sx + cos * sy; // = d
            this->matrix[12]                    = x - ox * this->matrix[0] - oy * this->matrix[4];
            this->matrix[13]                    = y - ox * this->matrix[1] - oy * this->matrix[5];
        }

        void Translate(float x, float y)
        {
            Matrix4 t {};
            t.SetTranslation(x, y);
            this->operator*=(t);
        }

        void Rotate(float rotation)
        {
            Matrix4 t {};
            t.SetRotation(rotation);
            this->operator*=(t);
        }

        void Scale(float sx, float sy)
        {
            Matrix4 t {};
            t.SetScale(sx, sy);
            this->operator*=(t);
        }

        void Shear(float kx, float ky)
        {
            Matrix4 t {};
            t.SetShear(kx, ky);
            this->operator*=(t);
        }

        bool IsAffine2DTransform() const
        {
            return fabsf(this->matrix[2] + this->matrix[3] + this->matrix[6] + this->matrix[7] +
                         this->matrix[8] + this->matrix[9] + this->matrix[11] + this->matrix[14]) <
                       0.00001f &&
                   fabsf(this->matrix[10] + this->matrix[15] - 2.0f) < 0.00001f;
        }

        bool IsAffine3DTransform() const;

        template<typename Vdst, typename Vsrc>
        void TransformXY0(Vdst*, const Vsrc*, int size) const;

        template<typename Vdst, typename Vsrc>
        /* transform Vector2 src into Vector2 dst */
        void TransformXY(std::span<Vdst> dst, std::span<const Vsrc> src, int size) const
        {
            static_assert(dst.size() == size, "Mismatching dst size");
            static_assert(src.size() == size, "Mismatching src size");

            for (int i = 0; i < size; i++)
            {
                float x = (this->matrix[0] * src[i].x) + (this->matrix[4] * src[i].y) + (0) +
                          (this->matrix[12]);

                float y = (this->matrix[1] * src[i].x) + (this->matrix[5] * src[i].y) + (0) +
                          (this->matrix[13]);

                dst[i].x = x;
                dst[i].y = y;
            }
        }

        template<typename Vdst, typename Vsrc>
        /* transform Vector3 src into Vector3 dst */
        void TransformXYZ(std::span<Vdst> dst, std::span<const Vsrc> src, int size) const
        {
            static_assert(dst.size() == size, "Mismatching dst size");
            static_assert(src.size() == size, "Mismatching src size");

            for (int i = 0; i < size; i++)
            {
                float x = (this->matrix[0] * src[i].x) + (this->matrix[4] * src[i].y) +
                          (this->matrix[8] * src[i].z) + (this->matrix[12]);

                float y = (this->matrix[1] * src[i].x) + (this->matrix[5] * src[i].y) +
                          (this->matrix[9] * src[i].z) + (this->matrix[13]);

                float z = (this->matrix[2] * src[i].x) + (this->matrix[6] * src[i].y) +
                          (this->matrix[10] * src[i].z) + (this->matrix[14]);

                dst[i].x = x;
                dst[i].y = y;
                dst[i].z = z;
            }
        }

        Matrix4 Inverse() const
        {
            Matrix4 inv;

            inv.matrix[0] =
                matrix[5] * matrix[10] * matrix[15] - matrix[5] * matrix[11] * matrix[14] -
                matrix[9] * matrix[6] * matrix[15] + matrix[9] * matrix[7] * matrix[14] +
                matrix[13] * matrix[6] * matrix[11] - matrix[13] * matrix[7] * matrix[10];

            inv.matrix[4] =
                -matrix[4] * matrix[10] * matrix[15] + matrix[4] * matrix[11] * matrix[14] +
                matrix[8] * matrix[6] * matrix[15] - matrix[8] * matrix[7] * matrix[14] -
                matrix[12] * matrix[6] * matrix[11] + matrix[12] * matrix[7] * matrix[10];

            inv.matrix[8] =
                matrix[4] * matrix[9] * matrix[15] - matrix[4] * matrix[11] * matrix[13] -
                matrix[8] * matrix[5] * matrix[15] + matrix[8] * matrix[7] * matrix[13] +
                matrix[12] * matrix[5] * matrix[11] - matrix[12] * matrix[7] * matrix[9];

            inv.matrix[12] =
                -matrix[4] * matrix[9] * matrix[14] + matrix[4] * matrix[10] * matrix[13] +
                matrix[8] * matrix[5] * matrix[14] - matrix[8] * matrix[6] * matrix[13] -
                matrix[12] * matrix[5] * matrix[10] + matrix[12] * matrix[6] * matrix[9];

            inv.matrix[1] =
                -matrix[1] * matrix[10] * matrix[15] + matrix[1] * matrix[11] * matrix[14] +
                matrix[9] * matrix[2] * matrix[15] - matrix[9] * matrix[3] * matrix[14] -
                matrix[13] * matrix[2] * matrix[11] + matrix[13] * matrix[3] * matrix[10];

            inv.matrix[5] =
                matrix[0] * matrix[10] * matrix[15] - matrix[0] * matrix[11] * matrix[14] -
                matrix[8] * matrix[2] * matrix[15] + matrix[8] * matrix[3] * matrix[14] +
                matrix[12] * matrix[2] * matrix[11] - matrix[12] * matrix[3] * matrix[10];

            inv.matrix[9] =
                -matrix[0] * matrix[9] * matrix[15] + matrix[0] * matrix[11] * matrix[13] +
                matrix[8] * matrix[1] * matrix[15] - matrix[8] * matrix[3] * matrix[13] -
                matrix[12] * matrix[1] * matrix[11] + matrix[12] * matrix[3] * matrix[9];

            inv.matrix[13] =
                matrix[0] * matrix[9] * matrix[14] - matrix[0] * matrix[10] * matrix[13] -
                matrix[8] * matrix[1] * matrix[14] + matrix[8] * matrix[2] * matrix[13] +
                matrix[12] * matrix[1] * matrix[10] - matrix[12] * matrix[2] * matrix[9];

            inv.matrix[2] =
                matrix[1] * matrix[6] * matrix[15] - matrix[1] * matrix[7] * matrix[14] -
                matrix[5] * matrix[2] * matrix[15] + matrix[5] * matrix[3] * matrix[14] +
                matrix[13] * matrix[2] * matrix[7] - matrix[13] * matrix[3] * matrix[6];

            inv.matrix[6] =
                -matrix[0] * matrix[6] * matrix[15] + matrix[0] * matrix[7] * matrix[14] +
                matrix[4] * matrix[2] * matrix[15] - matrix[4] * matrix[3] * matrix[14] -
                matrix[12] * matrix[2] * matrix[7] + matrix[12] * matrix[3] * matrix[6];

            inv.matrix[10] =
                matrix[0] * matrix[5] * matrix[15] - matrix[0] * matrix[7] * matrix[13] -
                matrix[4] * matrix[1] * matrix[15] + matrix[4] * matrix[3] * matrix[13] +
                matrix[12] * matrix[1] * matrix[7] - matrix[12] * matrix[3] * matrix[5];

            inv.matrix[14] =
                -matrix[0] * matrix[5] * matrix[14] + matrix[0] * matrix[6] * matrix[13] +
                matrix[4] * matrix[1] * matrix[14] - matrix[4] * matrix[2] * matrix[13] -
                matrix[12] * matrix[1] * matrix[6] + matrix[12] * matrix[2] * matrix[5];

            inv.matrix[3] =
                -matrix[1] * matrix[6] * matrix[11] + matrix[1] * matrix[7] * matrix[10] +
                matrix[5] * matrix[2] * matrix[11] - matrix[5] * matrix[3] * matrix[10] -
                matrix[9] * matrix[2] * matrix[7] + matrix[9] * matrix[3] * matrix[6];

            inv.matrix[7] =
                matrix[0] * matrix[6] * matrix[11] - matrix[0] * matrix[7] * matrix[10] -
                matrix[4] * matrix[2] * matrix[11] + matrix[4] * matrix[3] * matrix[10] +
                matrix[8] * matrix[2] * matrix[7] - matrix[8] * matrix[3] * matrix[6];

            inv.matrix[11] =
                -matrix[0] * matrix[5] * matrix[11] + matrix[0] * matrix[7] * matrix[9] +
                matrix[4] * matrix[1] * matrix[11] - matrix[4] * matrix[3] * matrix[9] -
                matrix[8] * matrix[1] * matrix[7] + matrix[8] * matrix[3] * matrix[5];

            inv.matrix[15] =
                matrix[0] * matrix[5] * matrix[10] - matrix[0] * matrix[6] * matrix[9] -
                matrix[4] * matrix[1] * matrix[10] + matrix[4] * matrix[2] * matrix[9] +
                matrix[8] * matrix[1] * matrix[6] - matrix[8] * matrix[2] * matrix[5];

            float det = matrix[0] * inv.matrix[0] + matrix[1] * inv.matrix[4] +
                        matrix[2] * inv.matrix[8] + matrix[3] * inv.matrix[12];

            float invdet = 1.0f / det;

            for (int i = 0; i < 16; i++)
                inv.matrix[i] *= invdet;

            return inv;
        }

        static Matrix4 Ortho(float left, float right, float bottom, float top, float near,
                             float far)
        {
            Matrix4 t {};

            t.matrix[0]  = 2.0f / (right - left);
            t.matrix[5]  = 2.0f / (top - bottom);
            t.matrix[10] = -2.0f / (far - near);

            t.matrix[12] = -(right + left) / (right - left);
            t.matrix[13] = -(top + bottom) / (top - bottom);
            t.matrix[14] = -(far + near) / (far - near);

            return t;
        }

        static Matrix4 Perspective(float verticalfov, float aspect, float near, float far)
        {
            Matrix4 t {};

            float cotangent = 1.0f / tanf(verticalfov * 0.5f);

            t.matrix[0]  = cotangent / aspect;
            t.matrix[5]  = cotangent;
            t.matrix[10] = (far + near) / (near - far);
            t.matrix[11] = -1.0f;

            t.matrix[14] = 2.0f * near * far / (near - far);
            t.matrix[15] = 0.0f;

            return t;
        }

      protected:
        Elements matrix;
    };
} // namespace love
