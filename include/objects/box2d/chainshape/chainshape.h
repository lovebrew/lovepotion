#pragma once

#include "edgeshape/edgeshape.h"
#include "shape/shape.h"

namespace love
{
    class ChainShape : public Shape
    {
      public:
        static love::Type type;

        ChainShape(b2ChainShape* chainShape, bool own = true);

        virtual ~ChainShape();

        void SetNextVertex(float x, float y);

        b2Vec2 GetNextVertex() const;

        void SetPreviousVertex(float x, float y);

        b2Vec2 GetPreviousVertex() const;

        EdgeShape* GetChildEdge(int index) const;

        int GetVertexCount() const;

        b2Vec2 GetPoint(int index) const;

        const b2Vec2* GetPoints() const;
    };
} // namespace love
