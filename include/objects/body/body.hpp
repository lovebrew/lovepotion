#pragma once

#include <common/luax.hpp>
#include <common/math.hpp>
#include <common/object.hpp>
#include <common/reference.hpp>

#include <box2d/box2d.h>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    class World;
    class Shape;

    class Body : public Object
    {
      public:
        static love::Type type;

        friend class Joint;
        friend class DistanceJoint;
        friend class MouseJoint;
        friend class CircleShape;
        friend class PolygonShape;
        friend class Shape;

        enum Type
        {
            BODY_INVALID,
            BODY_STATIC,
            BODY_DYNAMIC,
            BODY_KINEMATIC,
            BODY_MAX_ENUM
        };

        Body(World* world, b2Vec2 position, Body::Type type);

        virtual ~Body();

        float GetX();

        float GetY();

        float GetAngle();

        void GetPosition(float& x, float& y);

        void GetLinearVelocity(float& x, float& y);

        void GetWorldCenter(float& x, float& y);

        void GetLocalCenter(float& x, float& y);

        float GetAngularVelocity();

        void GetKinematicState(b2Vec2& position, float& angle, b2Vec2& velocity,
                               float& angularVelocity) const;

        float GetMass() const;

        float GetInertia() const;

        int GetMassData(lua_State* L);

        bool HasCustomMassData() const
        {
            return this->hasCustomMass;
        }

        float GetAngularDamping() const;

        float GetLinearDamping() const;

        float GetGravityScale() const;

        Body::Type GetType() const;

        void ApplyLinearImpulse(float x, float y, bool wake);

        void ApplyLinearImpulse(float x, float y, float offsetX, float offsetY, bool wake);

        void ApplyAngularImpulse(float impulse, bool wake);

        void ApplyTorque(float torque, bool wake);

        void ApplyForce(float x, float y, bool wake);

        void ApplyForce(float x, float y, float offsetX, float offsetY, bool wake);

        void SetX(float x);

        void SetY(float y);

        void SetLinearVelocity(float x, float y);

        void SetAngle(float angle);

        void SetAngularVelocity(float rotation);

        void SetKinematicState(b2Vec2 position, float angle, b2Vec2 velocity,
                               float angularVelocity);

        void SetPosition(float x, float y);

        void ResetMassData();

        void SetMassData(float x, float y, float mass, float inertia);

        void SetMass(float mass);

        void SetInertia(float inertia);

        void SetAngularDamping(float damping);

        void SetLinearDamping(float damping);

        void SetGravityScale(float scale);

        void SetType(Body::Type type);

        void GetWorldPoint(float x, float y, float& outX, float& outY) const;

        void GetWorldVector(float x, float y, float& outX, float& outY) const;

        int GetWorldPoints(lua_State* L);

        void GetLocalPoint(float x, float y, float& outX, float& outY) const;

        void GetLocalVector(float x, float y, float& outX, float& outY) const;

        int GetLocalPoints(lua_State* L);

        void GetLinearVelocityFromWorldPoint(float x, float y, float& outX, float& outY) const;

        void GetLinearVelocityFromLocalPoint(float x, float y, float& outX, float& outY) const;

        bool IsBullet() const;

        void SetBullet(bool bullet);

        bool IsEnabled() const;

        bool IsAwake() const;

        void SetSleepingAllowed(bool allowed);

        bool IsSleepingAllowed() const;

        void SetEnabled(bool enabled);

        void SetAwake(bool awake);

        void SetFixedRotation(bool fixed);

        bool IsFixedRotation() const;

        bool IsTouching(Body* other) const;

        World* GetWorld() const;

        Shape* GetShape() const;

        int GetShapes(lua_State* L) const;

        int GetJoints(lua_State* L) const;

        int GetContacts(lua_State* L) const;

        void Destroy();

        int SetUserdata(lua_State* L);

        int GetUserdata(lua_State* L);

        b2Body* body;

        // clang-format off
        static constexpr BidirectionalMap bodyTypes =
        {
            "static",    BODY_STATIC,
            "dynamic",   BODY_DYNAMIC,
            "kinematic", BODY_KINEMATIC
        };
        // clang-format on

      private:
        World* world;
        bool hasCustomMass;
        Reference* reference = nullptr;
    };
} // namespace love
