#pragma once

#include "common/luax.h"
#include "common/reference.h"

#include "objects/box2d/world/world.h"

namespace love
{
    class QueryCallback : public b2QueryCallback
    {
      public:
        QueryCallback(World* world, lua_State* L, int index);

        ~QueryCallback();

        virtual bool ReportFixture(b2Fixture* fixture);

      private:
        World* world;
        lua_State* L;
        int functionIndex;
    };
} // namespace love
