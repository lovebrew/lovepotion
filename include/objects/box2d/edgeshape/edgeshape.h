#pragma once

#include "shape/shape.h"

namespace love
{
    class EdgeShape : public Shape
    {
      public:
        static love::Type type;

        EdgeShape(b2EdgeShape* shape, bool own = true);

        virtual ~EdgeShape();

        void SetNextVertex(float x, float y);

        b2Vec2 GetNextVertex() const;

        void SetPreviousVertex(float x, float y);

        b2Vec2 GetPreviousVertex() const;

        int GetPoints(lua_State* L);
    };
} // namespace love
