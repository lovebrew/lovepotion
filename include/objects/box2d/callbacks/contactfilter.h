#pragma once

#include "common/luax.h"
#include "common/reference.h"

namespace love
{
    class Fixture;

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
