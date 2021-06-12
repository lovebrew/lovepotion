#pragma once

#include "common/luax.h"
#include "common/stringmap.h"

#include "objects/object.h"

#include <box2d/box2d.h>

namespace love
{
    class Shape : public Object
    {
      public:
        static love::Type type;

        enum Type
        {
            SHAPE_INVALID,
            SHAPE_CIRCLE,
            SHAPE_POLYGON,
            SHAPE_EDGE,
            SHAPE_CHAIN,
            SHAPE_MAX_ENUM
        };

        friend class Fixture;

        Shape();

        Shape(b2Shape* shape, bool own = true);

        virtual ~Shape();

        Shape::Type GetType() const;

        float GetRadius() const;

        int GetChildCount() const;

        bool TestPoint(float x, float y, float radius, float px, float py) const;

        int RayCast(lua_State* L) const;

        int ComputeAABB(lua_State* L) const;

        int ComputeMass(lua_State* L) const;

        static bool GetConstant(const char* in, Shape::Type& out);
        static bool GetConstant(Shape::Type in, const char*& out);
        static std::vector<const char*> GetConstants(Shape::Type);

      protected:
        b2Shape* shape;
        bool own;

      private:
        const static StringMap<Shape::Type, SHAPE_MAX_ENUM> types;
    };
} // namespace love
