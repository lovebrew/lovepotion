#pragma once

#include "common/luax.h"
#include "common/reference.h"

#include "objects/box2d/world/world.h"

namespace love
{
    class ContactFilter
    {
      public:
        Reference* ref;
        lua_State* L;

        ContactFilter();

        ~ContactFilter();

        bool Process(Fixture* a, Fixture* b);
    };
} // namespace love
