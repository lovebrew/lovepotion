#pragma once

#include <common/matrix.tcc>

#include <citro3d.h>

namespace love
{
    template<>
    class Matrix4<Console::CTR> : public Matrix4<Console::ALL>
    {
      private:
        static void Multiply(const Matrix4& a, const Matrix4& b, Elements& t);

      public:
        static void Multiply(const Matrix4& a, const Matrix4& b, Matrix4& result);

        Matrix4()
        {
            this->SetIdentity();
        }

        Matrix4(const Elements& elements);

        Matrix4(const Matrix4& a, const Matrix4& b);

        Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                float ky);

        void operator*=(const Matrix4& other);

        Matrix4 operator*(const Matrix4& other) const;

        const Elements* GetElements() const
        {
            return &this->matrix;
        }

        void SetRow(int row, const Vector4& vector);

        Vector4 GetRow(int row);

        void SetColumn(int column, const Vector4& vector);

        Vector4 GetColumn(int column);

        void SetIdentity();

        void SetTranslation(float x, float y);

        void SetRotation(float rotation);

        void SetScale(float sx, float sy);

        void SetShear(float kx, float ky);

        void GetApproximateScale(float& sx, float& sy) const;

        void SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y);

        void SetTransformation(float x, float y, float angle, float sx, float sy, float ox,
                               float oy, float kx, float ky);

        void Translate(float x, float y);

        void Rotate(float rotation);

        void Scale(float sx, float sy);

        void Shear(float kx, float ky);

        bool IsAffine2DTransform() const;

        bool IsAffine3DTransform() const;

        Matrix4 Inverse() const;

        static Matrix4 Ortho(float left, float right, float bottom, float top, float near,
                             float far);

        static Matrix4 Perspective(float verticalfov, float aspect, float near, float far);
    };
} // namespace love
