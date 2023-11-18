#pragma once

#include "console.hpp"
#include "math.hpp"
#include "vector.hpp"

#include "utilities/driver/renderer/vertex.hpp"

#include <cstring>
#include <ranges>
#include <utility>

#include <span>

namespace love
{
    template<typename R, typename T>
    concept ValidTransformRange =
        std::ranges::random_access_range<R> &&
        (std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<R>>,
                        std::remove_cvref_t<T>> ||
         (std::is_same_v<std::remove_cvref_t<T>, Vector3> &&
          std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<R>>, vertex::Vertex>));

    template<typename R>
    concept Vector3TransformRange = ValidTransformRange<R, Vector3>;

    template<typename R>
    concept Vector2TransformRange = ValidTransformRange<R, Vector2>;

    template<typename R>
    concept VectorAtLeast2TransformRange =
        ValidTransformRange<R, Vector2> || ValidTransformRange<R, Vector3>;

    class Matrix4
    {
      protected:
        template<VectorAtLeast2TransformRange R>
        static auto DeVertexize(R&& r) -> decltype(auto)
        {
            if constexpr (std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<R>>,
                                         vertex::Vertex>)
            {
                return r | std::views::transform([](auto&& e) -> auto& { return e.position; });
            }
            else
            {
                return std::forward<decltype(r)>(r);
            }
        }

        static void Multiply(const Matrix4& a, const Matrix4& b, float t[0x10]);

      public:
        static void Multiply(const Matrix4& a, const Matrix4& b, Matrix4& result);

        Matrix4();

        Matrix4(float t00, float t10, float t01, float t11, float x, float y);

        Matrix4(const float elements[0x10]);

        Matrix4(const Matrix4& a, const Matrix4& b);

        Matrix4(float x, float y, float angle, float sx, float sy, float ox, float oy, float kx,
                float ky);

        Matrix4 operator*(const Matrix4& other) const;

        void operator*=(const Matrix4& other);

        const float* GetElements() const
        {
            return this->matrix;
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

        template<Vector3TransformRange Vdst, VectorAtLeast2TransformRange Vsrc>
        void TransformXY0(Vdst&&, const Vsrc&&, int size) const;

        template<VectorAtLeast2TransformRange Vdst, VectorAtLeast2TransformRange Vsrc>
        /* transform Vector2 src into Vector2 dst */
        void TransformXY(Vdst&& dst, Vsrc&& src) const
        {
            if constexpr (std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<Vdst>>,
                                         vertex::Vertex> ||
                          std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<Vsrc>>,
                                         vertex::Vertex>)
            {
                TransformXY(DeVertexize(dst), DeVertexize(src));
                return;
            }
            else
            {
                // This might need to be an assert; jury's out
                // static_assert(std::ranges::size(dst) == std::ranges::size(src));

                for (size_t i = 0; i < std::ranges::size(dst); i++)
                {
                    float x = (this->matrix[0] * src[i].x) + (this->matrix[4] * src[i].y) + (0) +
                              (this->matrix[12]);

                    float y = (this->matrix[1] * src[i].x) + (this->matrix[5] * src[i].y) + (0) +
                              (this->matrix[13]);

                    dst[i].x = x;
                    dst[i].y = y;
                }
            }
        }

        template<Vector3TransformRange Vdst, Vector3TransformRange Vsrc>
        /* transform Vector3 src into Vector3 dst */
        void TransformXYZ(Vdst&& dst, Vsrc&& src) const
        {
            if constexpr (std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<Vdst>>,
                                         vertex::Vertex> ||
                          std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<Vsrc>>,
                                         vertex::Vertex>)
            {
                TransformXYZ(DeVertexize(dst), DeVertexize(src));
                return;
            }
            else
            {
                // static_assert(std::ranges::size(dst) == std::ranges::size(src));

                for (size_t i = 0; i < std::ranges::size(dst); i++)
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
        }

        Matrix4 Inverse() const;

        static Matrix4 Ortho(float left, float right, float bottom, float top, float near,
                             float far);

        static Matrix4 Perspective(float verticalfov, float aspect, float near, float far);

      protected:
        float matrix[0x10];
    };
} // namespace love
