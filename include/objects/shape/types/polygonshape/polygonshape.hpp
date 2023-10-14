#pragma once

#include <objects/shape/shape.hpp>

namespace love
{
    class PolygonShape : public Shape
    {
      public:
        static love::Type type;

        PolygonShape(Body* body, const b2PolygonShape& polygonShape);

        virtual ~PolygonShape();

        int GetPoints(lua_State* L);

        bool Validate() const;
    };
} // namespace love
