#pragma once

#include "common/stringmap.h"
#include "objects/object.h"

#include "common/reference.h"

#include <box2d/box2d.h>

namespace love
{
    class World;
    class Shape;
    class Fixture;

    class Body : public Object
    {
      public:
        static love::Type type;

        enum Type
        {
            BODY_INVALID,
            BODY_STATIC,
            BODY_DYNAMIC,
            BODY_KINEMATIC,
            BODY_MAX_ENUM
        };

        struct BodyUserdata
        {
            Reference* ref = nullptr;
        };

        friend class Joint;
        friend class DistanceJoint;
        friend class MouseJoint;
        friend class CircleShape;
        friend class PolygonShape;
        friend class Shape;
        friend class Fixture;

        /* box2d relevant methods */

        Body(World* world, b2Vec2 position, Body::Type type);

        virtual ~Body();

        float GetX();

        float GetY();

        float GetAngle();

        void GetPosition(float& xPosition, float& yPosition);

        void GetLinearVelocity(float& xVelocity, float& yVelocity);

        void GetWorldCenter(float& xMassCenter, float& yMassCenter);

        void GetLocalCenter(float& xMassCenter, float& yMassCenter);

        float GetAngularVelocity() const;

        bool IsEnabled() const;

        void SetEnabled(bool enabled);

        int SetUserData(lua_State* L);

        int GetUserData(lua_State* L);

        void GetKinematicState(b2Vec2& outPosition, float& outAngle, b2Vec2& outVelocity,
                               float& outAngularVelocity) const;

        void SetKinematicState(b2Vec2 pos, float a, b2Vec2 vel, float da);

        float GetMass() const;

        float GetInertia() const;

        int GetMassData(lua_State* L);

        float GetAngularDamping() const;

        float GetLinearDamping() const;

        float GetGravityScale() const;

        Body::Type GetType() const;

        void ApplyLinearImpulse(float xImpulse, float yImpulse, bool wake);

        void ApplyLinearImpulse(float xImpulse, float yImpulse, float xOffset, float yOffset,
                                bool wake);

        void ApplyAngularImpulse(float impulse, bool wake);

        void ApplyTorque(float torque, bool wake);

        void ApplyForce(float xForce, float yForce, bool wake);

        void ApplyForce(float xForce, float yForce, float xOffset, float yOffset, bool wake);

        void SetX(float x);

        void SetY(float y);

        void SetLinearVelocity(float x, float y);

        void SetAngle(float angle);

        void SetAngularVelocity(float spin);

        void SetPosition(float x, float y);

        void ResetMassData();

        void SetMassData(float x, float y, float mass, float inertia);

        void SetMass(float mass);

        void SetInertia(float inertia);

        void SetAngularDamping(float damping);

        void SetLinearDamping(float damping);

        void SetGravityScale(float scale);

        void SetType(Body::Type type);

        void GetWorldPoint(float x, float y, float& xWorld, float& yWorld);

        void GetWorldVector(float x, float y, float& xWorld, float& yWorld);

        int GetWorldPoints(lua_State* L);

        void GetLocalPoint(float x, float y, float& xLocal, float& yLocal);

        void GetLocalVector(float x, float y, float& xLocal, float& yLocal);

        int GetLocalPoints(lua_State* L);

        void GetLinearVelocityFromWorldPoint(float x, float y, float& xVelocity, float& yVelocity);

        void GetLinearVelocityFromLocalPoint(float x, float y, float& xVelocity, float& yVelocity);

        bool IsBullet() const;

        void SetBullet(bool bullet);

        bool IsActive() const;

        bool IsAwake() const;

        void SetSleepingAllowed(bool allowed);

        bool IsSleepingAllowed() const;

        void SetActive(bool active);

        void SetAwake(bool awake);

        void SetFixedRotation(bool fixed);

        bool IsFixedRotation() const;

        bool IsTouching(Body* other) const;

        World* GetWorld() const;

        int GetFixtures(lua_State* L) const;

        int GetJoints(lua_State* L) const;

        int GetContacts(lua_State* L) const;

        void Destroy();

        int SetUserdata(lua_State* L);

        int GetUserdata(lua_State* L);

        /* public because joints, etc request it */
        b2Body* body;

        static bool GetConstant(const char* in, Body::Type& out);
        static bool GetConstant(Body::Type in, const char*& out);
        static std::vector<const char*> GetConstants(Body::Type);

      private:
        World* world;
        BodyUserdata* userdata;

        const static StringMap<Body::Type, BODY_MAX_ENUM> types;
    };
} // namespace love
