#pragma once

#include <common/luax.hpp>
#include <common/object.hpp>
#include <common/reference.hpp>

#include <unordered_map>
#include <vector>

#include <box2d/box2d.h>

namespace love
{
    class Contact;
    class Body;
    class Shape;
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
        friend class Shape;

        static love::Type type;

        class ContactCallback
        {
          public:
            Reference* reference;
            lua_State* state;
            World* world;

            ContactCallback(World* world);

            void Process(b2Contact* contact, const b2ContactImpulse* impulse = nullptr);

            ~ContactCallback();
        };

        class ContactFilter
        {
          public:
            Reference* reference;
            lua_State* state;

            ContactFilter();

            ~ContactFilter();

            bool Process(Shape* a, Shape* b);
        };

        class QueryCallback : public b2QueryCallback
        {
          public:
            QueryCallback(World* world, lua_State* L, int index);

            virtual ~QueryCallback();

            bool ReportFixture(b2Fixture* fixture) override;

          private:
            World* world;
            lua_State* state;
            int funcIndex;
            int userArgs;
        };

        class CollectCallback : public b2QueryCallback
        {
          public:
            CollectCallback(World* world, uint16_t mask, lua_State* L);

            virtual ~CollectCallback();

            bool ReportFixture(b2Fixture* fixture) override;

          private:
            World* world;
            uint16_t categoryMask;
            lua_State* state;
            int index = 1;
        };

        class RayCastCallback : public b2RayCastCallback
        {
          public:
            RayCastCallback(World* world, lua_State* L, int index);

            virtual ~RayCastCallback();

            float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal,
                                float fraction) override;

          private:
            World* world;
            lua_State* state;
            int funcIndex;
            int userArgs;
        };

        class RayCastOneCallback : public b2RayCastCallback
        {
          public:
            RayCastOneCallback(uint16_t mask, bool any);

            virtual ~RayCastOneCallback()
            {}

            float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal,
                                float fraction) override;

            b2Fixture* hitFixture;
            b2Vec2 hitPoint;
            b2Vec2 hitNormal;
            float hitFraction;

          private:
            uint16_t categoryMask;
            bool any;
        };

        World();

        World(b2Vec2 gravity, bool sleep);

        virtual ~World();

        void Update(float delta);

        void Update(float delta, int velocityIterations, int positionIterations);

        void BeginContact(b2Contact* contact);

        void EndContact(b2Contact* contact);

        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

        bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB);

        void SayGoodbye(b2Fixture* fixture);

        void SayGoodbye(b2Joint* joint);

        bool IsValid() const;

        int SetCallbacks(lua_State* L);

        int GetCallbacks(lua_State* L);

        void SetCallbacksL(lua_State* L);

        int SetContactFilter(lua_State* L);

        int GetContactFilter(lua_State* L);

        void SetGravity(float x, float y);

        int GetGravity(lua_State* L);

        void TranslateOrigin(float x, float y);

        void SetSleepingAllowed(bool allowed);

        bool IsSleepingAllowed() const;

        bool IsLocked() const;

        int GetBodyCount() const;

        int GetJointCount() const;

        int GetContactCount() const;

        int GetBodies(lua_State* L) const;

        int GetJoints(lua_State* L) const;

        int GetContacts(lua_State* L);

        b2Body* GetGroundBody() const;

        int QueryShapesInArea(lua_State* L);

        int GetShapesInArea(lua_State* L);

        int RayCast(lua_State* L);

        int RayCastAny(lua_State* L);

        int RayCastClosest(lua_State* L);

        void Destroy();

        void RegisterObject(void* b2Object, Object* object);

        void UnregisterObject(void* b2Object);

        Object* FindObject(void* b2Object) const;

      private:
        b2World* world;
        b2Body* groundBody;

        std::vector<Body*> destructBodies;
        std::vector<Shape*> destructShapes;
        std::vector<Joint*> destructJoints;

        bool destructWorld;

        ContactCallback begin, end, preSolve, postSolve;
        ContactFilter filter;

        std::unordered_map<void*, Object*> box2dObjectMap;
    };
} // namespace love
