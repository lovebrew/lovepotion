#pragma once

#include <objects/shape/shape.hpp>

namespace love
{
    class EdgeShape : public Shape
    {
      public:
        static love::Type type;

        EdgeShape(Body* body, const b2EdgeShape& e);

        virtual ~EdgeShape();

        void SetNextVertex(float x, float y);

        b2Vec2 GetNextVertex() const;

        void SetPreviousVertex(float x, float y);

        b2Vec2 GetPreviousVertex() const;

        int GetPoints(lua_State* L);
    };
} // namespace love
