#pragma once

#include <common/matrix.tcc>

#include <citro2d.h>

#include <cstring>

namespace love
{
    template<>
    class Matrix4<Console::CTR> : public Matrix4<Console::ALL>
    {
      public:
        Matrix4();

        Matrix4(const C3D_Mtx& matrix);

        Matrix4(const float (&elements)[16], bool columnMajor)
        {
            static_assert(sizeof(elements) == sizeof(float[16]));
            std::memcpy(this->matrix.m, elements, sizeof(elements));

            if (columnMajor)
                Mtx_Transpose(&this->matrix);
        }

        Matrix4(const Matrix4& a, const Matrix4& b);

        Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                float ky);

        Matrix4(float t00, float t10, float t01, float t11, float x, float y);

        const C3D_Mtx& GetElements() const
        {
            return this->matrix;
        }

        void SetIdentity();

        void SetTranslation(float x, float y);

        void Translate(float x, float y);

        void Rotate(float r);

        void Scale(float sx, float sy);

        void Shear(float kx, float ky);

        bool IsAffine2DTransform() const;

        bool IsAffine3DTransform() const;

        Matrix4 Inverse() const;

        void SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y);

        void SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                               float oy, float kx, float ky);

        void Translation(float x, float y);

        void SetRotation(float r);

        void SetScale(float x, float y);

        void SetShear(float kx, float ky);

        void GetApproximateScale(float& sx, float& sy) const;

        Matrix4 operator*(const Matrix4& m) const;

        void operator*=(const Matrix4& m);

        static Matrix4 Ortho(float left, float right, float bottom, float top, float near,
                             float far);

        static void Multiply(const Matrix4& a, const Matrix4& b, Matrix4& result);

        float Get(const unsigned row, const unsigned column) const
        {
            return this->matrix.m[row * 4 + column];
        }

        void Set(const unsigned row, const unsigned column, const float value)
        {
            this->matrix.m[row * 4 + column] = value;
        }

        void TransformXY(const C3D_Mtx& elements);

        void TransformXY();

        /**
         * Transforms an array of 2-component vertices by this Matrix. The source
         * and destination arrays may be the same.
         **/
        template<typename Vdst, typename Vsrc>
        void TransformXY(Vdst* dst, const Vsrc* src, int size) const;

        /**
         * Transforms an array of 2-component vertices by this Matrix, and stores
         * them in an array of 3-component vertices.
         **/
        template<typename Vdst, typename Vsrc>
        void TransformXY0(Vdst* dst, const Vsrc* src, int size) const;

      private:
        static void Multiply(const Matrix4& a, const Matrix4& b, C3D_Mtx& c);

        C3D_Mtx matrix;
    };

    template<typename Vdst, typename Vsrc>
    void Matrix4<Console::CTR>::TransformXY(Vdst* dst, const Vsrc* src, int size) const
    {
        for (int i = 0; i < size; i++)
        {
            // Store in temp variables in case src = dst

            // clang-format off
            float x = (this->matrix.r[0].x * src[i].x) + (this->matrix.r[0].y * src[i].y) + (0) + (this->matrix.r[0].w);
            float y = (this->matrix.r[1].x * src[i].x) + (this->matrix.r[1].y * src[i].y) + (0) + (this->matrix.r[1].w);
            // clang-format on

            dst[i].x = x;
            dst[i].y = y;
        }
    }
} // namespace love
