#pragma once

#include <common/object.hpp>
#include <common/reference.hpp>

#include <objects/body/body.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <box2d/box2d.h>

namespace love
{
    class Shape : public Object
    {
      public:
        static love::Type type;

        friend class Physics;

        enum Type
        {
            SHAPE_INVALID,
            SHAPE_CIRCLE,
            SHAPE_POLYGON,
            SHAPE_EDGE,
            SHAPE_CHAIN,
            SHAPE_MAX_ENUM
        };

        Shape(Body* body, const b2Shape& shape);

        virtual ~Shape();

        void Destroy(bool implicit = false);

        bool IsValid() const
        {
            return this->fixture != nullptr;
        }

        bool IsShapeValid() const
        {
            return this->shape != nullptr;
        }

        bool IsSensor() const;

        void SetSensor(bool sensor);

        Body* GetBody() const;

        void SetFilterData(int* data);

        void GetFilterData(int* data);

        int SetUserdata(lua_State* L);

        int GetUserdata(lua_State* L);

        void SetFriction(float friction);

        void SetRestitution(float restitution);

        void SetDensity(float density);

        float GetFriction() const;

        float GetRestitution() const;

        float GetDensity() const;

        bool TestPoint(float x, float y) const;

        bool TestPoint(float x, float y, float angle, float cx, float cy) const;

        Shape::Type GetType() const;

        float GetRadius() const;

        int GetChildCount() const;

        int RayCast(lua_State* L) const;

        int ComputeAABB(lua_State* L) const;

        int ComputeMass(lua_State* L) const;

        void SetGroupIndex(int index);

        int GetGroupIndex() const;

        int SetCategory(lua_State* L);

        int SetMask(lua_State* L);

        int GetCategory(lua_State* L);

        int GetMask(lua_State* L);

        uint16_t GetCategoryBits(lua_State* L) const;

        int PushCategoryBits(lua_State* L, uint16_t bits);

        int GetBoundingBox(lua_State* L) const;

        int GetMassData(lua_State* L) const;

        void ThrowIfFixtureNotValid() const;

        void ThrowIfShapeNotValid() const;

        // clang-format off
        static constexpr BidirectionalMap shapeTypes =
        {
            "circle",  SHAPE_CIRCLE,
            "polygon", SHAPE_POLYGON,
            "edge",    SHAPE_EDGE,
            "chain",   SHAPE_CHAIN
        };
        // clang-format on

      protected:
        b2Shape* shape;
        bool own;

        Shape::Type shapeType;
        Body* body;

        b2Fixture* fixture;
        Reference* reference = nullptr;
    };
} // namespace love
