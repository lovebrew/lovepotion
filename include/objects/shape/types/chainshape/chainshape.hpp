#pragma once

#include <objects/shape/shape.hpp>
#include <objects/shape/types/edgeshape/edgeshape.hpp>

namespace love
{
    class ChainShape : public Shape
    {
      public:
        static love::Type type;

        ChainShape(Body* body, const b2ChainShape& c);

        virtual ~ChainShape();

        void SetNextVertex(float x, float y);

        void SetPreviousVertex(float x, float y);

        b2Vec2 GetNextVertex() const;

        b2Vec2 GetPreviousVertex() const;

        EdgeShape* GetChildEdge(int index) const;

        int GetVertexCount() const;

        b2Vec2 GetPoint(int index) const;

        const b2Vec2* GetPoints() const;
    };
} // namespace love
