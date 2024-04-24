#pragma once

#include "common/Map.hpp"
#include "common/Matrix.hpp"
#include "common/Object.hpp"
#include "common/Vector.hpp"

namespace love
{
    class Transform : public Object
    {
      public:
        enum MatrixLayout
        {
            MATRIX_ROW_MAJOR,
            MATRIX_COLUMN_MAJOR,
            MATRIX_MAX_ENUM
        };

        static Type type;

        Transform();

        Transform(const Matrix4& m);

        Transform(float x, float y, float a, float sx, float sy, float ox, float oy, float kx, float ky);

        virtual ~Transform();

        Transform* clone();

        Transform* inverse();

        void apply(Transform* other);

        void translate(float x, float y);

        void rotate(float angle);

        void scale(float x, float y);

        void shear(float x, float y);

        void reset();

        void setTransformation(float x, float y, float a, float sx, float sy, float ox, float oy, float kx,
                               float ky);

        Vector2 transformPoint(Vector2 point) const;

        Vector2 inverseTransformPoint(Vector2 point);

        const Matrix4& getMatrix() const;

        void setMatrix(const Matrix4& m);

        // clang-format off
        STRINGMAP_DECLARE(MatrixLayouts, MatrixLayout,
            { "row",    MATRIX_ROW_MAJOR    },
            { "column", MATRIX_COLUMN_MAJOR }
        );
        // clang-format on

      private:
        inline const Matrix4& getInverseMatrix()
        {
            if (this->inverseDirty)
            {
                this->inverseMatrix = matrix.inverse();
                this->inverseDirty  = false;
            }

            return this->inverseMatrix;
        }

        Matrix4 matrix;
        bool inverseDirty;
        Matrix4 inverseMatrix;
    };
} // namespace love
