#pragma once

#include "common/luax.h"
#include "common/reference.h"

#include <box2d/box2d.h>

namespace love
{
    class World;

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
