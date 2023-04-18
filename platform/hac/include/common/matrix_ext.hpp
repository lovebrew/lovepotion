#pragma once

#include <common/matrix.tcc>

namespace love
{
    template<>
    class Matrix4<Console::HAC> : public Matrix4<Console::ALL>
    {
      public:
        Matrix4();

        Matrix4(const float elements[16]);

        Matrix4(const Matrix4& a, const Matrix4& b);

        Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                float ky);

        Matrix4(float t00, float t10, float t01, float t11, float x, float y);

        void SetIdentity();

        void Transpose()
        {}

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
            return this->elements[column * 4 + row];
        }

        void Set(const unsigned row, const unsigned column, const float value)
        {
            this->elements[column * 4 + row] = value;
        }

        void TransformXY(float elements[16]);

        void TransformXY();

        /**
         * Transforms an array of 2-component vertices by this Matrix. The source
         * and destination arrays may be the same.
         **/
        template<typename Vdst, typename Vsrc>
        void TransformXY(Vdst* dst, const Vsrc* src, int size) const;

        template<typename Vdst, typename Vsrc>
        void TransformXYVert(Vdst* dst, const Vsrc* src, int size) const;

        template<typename Vdst, typename Vsrc>
        void TransformXYVertPure(Vdst* dst, const Vsrc* src, int size) const;

        /**
         * Transforms an array of 2-component vertices by this Matrix, and stores
         * them in an array of 3-component vertices.
         **/
        template<typename Vdst, typename Vsrc>
        void TransformXY0(Vdst* dst, const Vsrc* src, int size) const;

      private:
        static void Multiply(const Matrix4& a, const Matrix4& b, float elements[16]);

        float elements[16];
    };

    /* use with Vector2 */
    template<typename Vdst, typename Vsrc>
    void Matrix4<Console::HAC>::TransformXY(Vdst* dst, const Vsrc* src, int size) const
    {
        for (int i = 0; i < size; i++)
        {
            // Store in temp variables in case src = dst
            float x = (this->elements[0] * src[i].x) + (this->elements[4] * src[i].y) + (0) +
                      (this->elements[12]);
            float y = (this->elements[1] * src[i].x) + (this->elements[5] * src[i].y) + (0) +
                      (this->elements[13]);

            dst[i].x = x;
            dst[i].y = y;
        }
    }

    /* use with Vertex */
    template<typename Vdst, typename Vsrc>
    void Matrix4<Console::HAC>::TransformXYVert(Vdst* dst, const Vsrc* src, int size) const
    {
        for (int i = 0; i < size; i++)
        {
            // Store in temp variables in case src = dst
            float x = (this->elements[0] * src[i].position[0]) +
                      (this->elements[4] * src[i].position[1]) + (0) + (this->elements[12]);

            float y = (this->elements[1] * src[i].position[0]) +
                      (this->elements[5] * src[i].position[1]) + (0) + (this->elements[13]);

            dst[i].x = x;
            dst[i].y = y;
        }
    }

    /* use with Vertex */
    template<typename Vdst, typename Vsrc>
    void Matrix4<Console::HAC>::TransformXYVertPure(Vdst* dst, const Vsrc* src, int size) const
    {
        for (int i = 0; i < size; i++)
        {
            // Store in temp variables in case src = dst
            float x = (this->elements[0] * src[i].position[0]) +
                      (this->elements[4] * src[i].position[1]) + (0) + (this->elements[12]);

            float y = (this->elements[1] * src[i].position[0]) +
                      (this->elements[5] * src[i].position[1]) + (0) + (this->elements[13]);

            dst[i].position[0] = x;
            dst[i].position[1] = y;
        }
    }
} // namespace love
