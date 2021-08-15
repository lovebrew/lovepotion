#pragma once

#if defined(__3DS__)
    #include <citro2d.h>
typedef C3D_Mtx Elements;
#elif defined(__SWITCH__)
typedef float Elements[16];
#endif

#include "common/lmath.h"

namespace love
{
    class Matrix4
    {
      public:
        Matrix4();

        Matrix4(const Elements& matrix);

        Matrix4(const Matrix4& a, const Matrix4& b);

        Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                float ky);

        Matrix4(float t00, float t10, float t01, float t11, float x, float y);

        const Elements& GetElements() const;

        void SetIdentity();

        void SetTranslation(float x, float y);

        void Translate(float x, float y);

        void Rotate(float r);

        void Scale(float sx, float sy);

        void Shear(float kx, float ky);

        bool IsAffine2DTransform() const;

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

        void TransformXY(const Elements& elements);

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

        Elements matrix;

      private:
        static void Multiply(const Matrix4& a, const Matrix4& b, Elements& c);
    };

#if defined(__SWITCH__)
    template<typename Vdst, typename Vsrc>
    void Matrix4::TransformXY(Vdst* dst, const Vsrc* src, int size) const
    {
        for (int i = 0; i < size; i++)
        {
            // Store in temp variables in case src = dst
            float x = (this->matrix[0] * src[i].x) + (this->matrix[4] * src[i].y) + (0) +
                      (this->matrix[12]);
            float y = (this->matrix[1] * src[i].x) + (this->matrix[5] * src[i].y) + (0) +
                      (this->matrix[13]);

            dst[i].x = x;
            dst[i].y = y;
        }
    }

    template<typename Vdst, typename Vsrc>
    void Matrix4::TransformXY0(Vdst* dst, const Vsrc* src, int size) const
    {
        for (int i = 0; i < size; i++)
        {
            // Store in temp variables in case src = dst
            float x = (this->matrix[0] * src[i].x) + (this->matrix[4] * src[i].y) + (0) +
                      (this->matrix[12]);
            float y = (this->matrix[1] * src[i].x) + (this->matrix[5] * src[i].y) + (0) +
                      (this->matrix[13]);
            float z = (this->matrix[2] * src[i].x) + (this->matrix[6] * src[i].y) + (0) +
                      (this->matrix[14]);

            dst[i].x = x;
            dst[i].y = y;
            dst[i].z = z;
        }
    }
#elif defined(__3DS__)
    template<typename Vdst, typename Vsrc>
    void Matrix4::TransformXY(Vdst* dst, const Vsrc* src, int size) const
    {
        for (int i = 0; i < size; i++)
        {
            // Store in temp variables in case src = dst
            float x = (this->matrix.r[0].x * src[i].x) + (this->matrix.r[0].y * src[i].y) + (0) +
                      (this->matrix.r[0].w);
            float y = (this->matrix.r[1].x * src[i].x) + (this->matrix.r[1].y * src[i].y) + (0) +
                      (this->matrix.r[1].w);

            dst[i].x = x;
            dst[i].y = y;
        }
    }
#endif
} // namespace love
