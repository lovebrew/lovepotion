#pragma once

#include "common/luax.h"
#include "common/reference.h"

#include <box2d/box2d.h>

namespace love
{
    class World;

    class ContactCallback
    {
      public:
        Reference* ref;
        lua_State* L;
        World* world;

        ContactCallback(World* world);

        ~ContactCallback();

        void Process(b2Contact* contact, const b2ContactImpulse* impulse = NULL);
    };
} // namespace love
