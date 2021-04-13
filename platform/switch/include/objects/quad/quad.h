#pragma once

#include "objects/quad/quadc.h"

namespace love
{
    class Quad : public common::Quad
    {
      public:
        Quad(const Viewport& viewport, double sw, double sh);

        virtual ~Quad()
        {}

        void Refresh(const Viewport& viewport, double sw, double sh) override;

        const Vector2* GetVertexPositions() const;
        const Vector2* GetVertexTexCoords() const;

      private:
        Vector2 vertexPositions[4];
        Vector2 vertexTexCoords[4];
    };
} // namespace love