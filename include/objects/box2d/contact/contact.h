#pragma once

#include "objects/object.h"
#include "world.h"

#include "modules/physics/physics.h"

#include <box2d/box2d.h>

namespace love
{
    class World;

    class Contact : public Object
    {
      public:
        friend class World;
        friend class ContactCallback;

        static love::Type type;

        Contact(World* world, b2Contact* contact);

        virtual ~Contact();

        void Invalidate();

        bool IsValid();

        int GetPositions(lua_State* L);

        int GetNormal(lua_State* L);

        float GetFriction() const;

        float GetRestitution() const;

        bool IsEnabled() const;

        bool IsTouching() const;

        void SetFriction(float friction);

        void SetRestitution(float restitution);

        void SetEnabled(bool enabled);

        void ResetFriction();

        void ResetRestitution();

        void SetTangentSpeed(float speed);

        float GetTangentSpeed() const;

        void GetChildren(int& childA, int& childB);

        void GetFixtures(Fixture*& fixtureA, Fixture*& fixtureB);

      private:
        b2Contact* contact;
        World* world;
    };
} // namespace love
