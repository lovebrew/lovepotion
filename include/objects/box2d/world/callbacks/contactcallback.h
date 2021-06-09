#pragma once

#include "common/luax.h"
#include "common/reference.h"

#include "objects/box2d/world/world.h"

namespace love
{
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
