#pragma once

#include "common/matrix.h"
#include "common/stringmap.h"
#include "common/vector.h"

#include "objects/object.h"

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

        static love::Type type;

        Transform();

        Transform(const Matrix4& matrix);

        Transform(float x, float y, float a, float sx, float sy, float ox, float oy, float kx,
                  float ky);

        virtual ~Transform() {};

        Transform* Clone();

        Transform* Inverse();

        void Apply(Transform* other);

        void Translate(float x, float y);

        void Rotate(float radians);

        void Scale(float sx, float sy);

        void Shear(float kx, float ky);

        void Reset();

        void SetTransformation(float x, float y, float a, float sx, float sy, float ox, float oy,
                               float kx, float ky);

        Vector2 TransformPoint(Vector2 point) const;

        Vector2 InverseTransformPoint(Vector2 point);

        const Matrix4& GetMatrix() const;

        void SetMatrix(const Matrix4& matrix);

        static bool GetConstant(const char* in, MatrixLayout& out);
        static bool GetConstant(MatrixLayout in, const char*& out);
        static std::vector<const char*> GetConstants(MatrixLayout);

      private:
        Matrix4 matrix;
        bool inverseDirty;
        Matrix4 inverseMatrix;

        inline const Matrix4& GetInverseMatrix()
        {
            if (this->inverseDirty)
            {
                this->inverseDirty  = false;
                this->inverseMatrix = this->matrix.Inverse();
            }

            return this->inverseMatrix;
        }

        const static StringMap<MatrixLayout, MATRIX_MAX_ENUM> matrixLayouts;
    };
} // namespace love
