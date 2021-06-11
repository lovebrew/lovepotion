#pragma once

#include "shape/shape.h"

namespace love
{
    class PolygonShape : public Shape
    {
      public:
        static love::Type type;

        PolygonShape(b2PolygonShape* polygonShape, bool own = true);

        virtual ~PolygonShape();

        int GetPoints(lua_State* L);

        bool Validate() const;
    };
} // namespace love
