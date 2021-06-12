#pragma once

#include "common/luax.h"
#include "common/reference.h"

#include <box2d/box2d.h>

namespace love
{
    class World;

    class RayCastCallback : public b2RayCastCallback
    {
      public:
        RayCastCallback(World* world, lua_State* L, int index);

        ~RayCastCallback();

        virtual float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal,
                                    float fraction);

      private:
        World* world;
        lua_State* L;
        int functionIndex;
    };
} // namespace love
