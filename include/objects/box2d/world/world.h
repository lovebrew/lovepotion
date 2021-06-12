#pragma once

#include "common/luax.h"
#include "common/reference.h"
#include "objects/object.h"

#include <unordered_map>
#include <vector>

#include "contactcallback.h"
#include "contactfilter.h"
#include "querycallback.h"
#include "raycastcallback.h"

#include <box2d/box2d.h>

namespace love
{
    class Contact;
    class Body;
    class Fixture;
    class Joint;

    class World : public Object,
                  public b2ContactListener,
                  public b2ContactFilter,
                  public b2DestructionListener
    {
      public:
        friend class Joint;
        friend class DistanceJoint;
        friend class MouseJoint;
        friend class Body;
        friend class Fixture;

        static love::Type type;

        World();

        World(b2Vec2 gravity, bool sleep);

        virtual ~World();

        void Update(float dt);

        void Update(float dt, int velocityIterations, int positionIterations);

        void BeginContact(b2Contact* contact);

        void EndContact(b2Contact* contact);

        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

        bool ShouldCollide(b2Fixture* a, b2Fixture* b);

        void SayGoodbye(b2Fixture* fixture);

        void SayGoodbye(b2Joint* joint);

        bool IsValid() const;

        int SetCallbacks(lua_State* L);

        int GetCallbacks(lua_State* L);

        void SetLuaCallbacks(lua_State* L);

        int SetContactFilter(lua_State* L);

        int GetContactFilter(lua_State* L);

        void SetGravity(float x, float y);

        int GetGravity(lua_State* L);

        void TranslateOrigin(float x, float y);

        void SetSleepingAllowed(bool allow);

        bool IsSleepingAllowed() const;

        bool IsLocked() const;

        int GetBodyCount() const;

        int GetJointCount() const;

        int GetContactCount() const;

        int GetBodies(lua_State* L) const;

        int GetJoints(lua_State* L) const;

        int GetContacts(lua_State* L);

        b2Body* GetGroundBody() const;

        int QueryBoundingBox(lua_State* L);

        int RayCast(lua_State* L);

        void Destroy();

        void RegisterObject(void* b2Object, love::Object* object);

        void UnRegisterObject(void* b2Object);

        love::Object* FindObject(void* b2Object) const;

      private:
        b2World* world;
        b2Body* groundBody;

        std::vector<Body*> destructBodies;
        std::vector<Fixture*> destructFixtures;
        std::vector<Joint*> destructJoints;

        bool destructWorld;

        ContactCallback begin, end, presolve, postsolve;
        ContactFilter filter;

        std::unordered_map<void*, love::Object*> box2dObjectMap;
    };
} // namespace love
