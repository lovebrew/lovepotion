#pragma once

#include <common/luax.hpp>
#include <common/object.hpp>

#include <objects/world/world.hpp>

#include <box2d/box2d.h>

namespace love
{
    class World;

    class Contact : public Object
    {
      public:
        friend class World;
        friend class World::ContactCallback;

        static Type type;

        Contact(World* world, b2Contact* contact);

        virtual ~Contact();

        void Invalidate();

        bool IsValid();

        int GetPositions(lua_State* L);

        int GetNormal(lua_State* L);

        float GetFriction() const;

        float GetRestitution() const;

        bool IsEnabled();

        bool IsTouching() const;

        void SetFriction(float friction);

        void SetRestitution(float restitution);

        void SetEnabled(bool enabled);

        void ResetFriction();

        void ResetRestitution();

        void SetTangentSpeed(float speed);

        float GetTangentSpeed() const;

        void GetChildren(int& childA, int& childB);

        void GetShapes(Shape*& shapeA, Shape*& shapeB);

      private:
        b2Contact* contact;
        World* world;
    };
} // namespace love
