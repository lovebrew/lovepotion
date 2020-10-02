#pragma once

#if defined (_3DS)
    typedef C3D_Mtx Elements;
#elif defined (__SWITCH__)
    typedef float Elements[16];
#endif

namespace love
{
    class Matrix4
    {
        public:
            Matrix4();

            Matrix4(const Elements & matrix);

            Matrix4(const Matrix4 & a, const Matrix4 & b);

            Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky);

            const Elements & GetElements() const;

            void SetIdentity();

            void SetTranslation(float x, float y);

            void Translate(float x, float y);

            void Rotate(float r);

            void Scale(float sx, float sy);

            void Shear(float kx, float ky);

            Matrix4 Inverse() const;

            void SetRawTransformation(float t00, float t10, float t01, float t11, float x, float y);

            void SetTransformation(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx, float ky);

            void Translation(float x, float y);

            void SetRotation(float r);

            void SetScale(float x, float y);

            void SetShear(float kx, float ky);

            void GetApproximateScale(float & sx, float & sy) const;

            Matrix4 operator * (const Matrix4 & m) const;

            void operator *= (const Matrix4 & m);

            static Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far);

            static void Multiply(const Matrix4 & a, const Matrix4 & b, Matrix4 & result);

            void TransformXY(const Elements & elements);

            void TransformXY();

            Elements matrix;

        private:
            static void Multiply(const Matrix4 & a, const Matrix4 & b, Elements & c);

    };
}
