#pragma once

#include "common/reference.h"
#include "objects/object.h"

#include <box2d/box2d.h>

#include "shape.h"

namespace love
{
    class World;
    class Body;

    class Fixture : public Object
    {
      public:
        friend class Physics;
        friend class Shape;

        struct FixtureUserdata
        {
            Reference* ref = nullptr;
        };

        static love::Type type;

        Fixture(Body* body, Shape* shape, float density);

        virtual ~Fixture();

        Shape::Type GetType();

        Shape* GetShape();

        bool IsValid() const;

        bool IsSensor() const;

        void SetSensor(bool sensor);

        Body* GetBody() const;

        void SetFilterData(int* filter);

        void GetFilterData(int* filter);

        int SetUserdata(lua_State* L);

        int GetUserdata(lua_State* L);

        void SetFriction(float friction);

        void SetRestitution(float restitution);

        void SetDensity(float density);

        float GetFriction() const;

        float GetRestitution() const;

        float GetDensity() const;

        bool TestPoint(float x, float y) const;

        int RayCast(lua_State* L) const;

        void SetGroupIndex(int index);

        int GetGroupIndex() const;

        int SetCategory(lua_State* L);

        int SetMask(lua_State* L);

        int GetCategory(lua_State* L);

        int GetMask(lua_State* L);

        uint16_t GetBits(lua_State* L);

        int PushBits(lua_State* L, uint16_t bits);

        int GetBoundingBox(lua_State* L) const;

        int GetMassData(lua_State* L) const;

        void Destroy(bool implicit = false);

      protected:
        void CheckCreateShape();

        Body* body;
        FixtureUserdata* userdata;
        b2Fixture* fixture;

        StrongReference<Shape> shape;
    };
} // namespace love
