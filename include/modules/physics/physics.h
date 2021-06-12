#pragma once

#include "common/exception.h"
#include "common/module.h"

#include "body.h"
#include "wrap_body.h"

#include "wrap_fixture.h"

#include "chainshape/chainshape.h"
#include "circleshape/circleshape.h"
#include "distancejoint/distancejoint.h"
#include "edgeshape/edgeshape.h"
#include "frictionjoint/frictionjoint.h"
#include "gearjoint/gearjoint.h"
#include "motorjoint/motorjoint.h"
#include "mousejoint/mousejoint.h"
#include "polygonshape/polygonshape.h"
#include "prismaticjoint/prismaticjoint.h"
#include "pulleyjoint/pulleyjoint.h"
#include "revolutejoint/revolutejoint.h"
#include "ropejoint/ropejoint.h"
#include "weldjoint/weldjoint.h"
#include "wheeljoint/wheeljoint.h"

namespace love
{
    class Physics : public Module
    {
      public:
        /* 30px = 1m by default */
        static constexpr int DEFAULT_METER = 30;

        Physics();

        virtual ~Physics();

        ModuleType GetModuleType() const
        {
            return M_PHYSICS;
        }

        const char* GetName() const override
        {
            return "love.physics";
        }

        static void SetMeter(float scale);

        static float GetMeter();

        static float ScaleDown(float scale);

        static float ScaleUp(float scale);

        static void ScaleDown(float& x, float& y);

        static void ScaleUp(float& x, float& y);

        static b2Vec2 ScaleDown(const b2Vec2& vector);

        static b2Vec2 ScaleUp(const b2Vec2& vector);

        static b2AABB ScaleDown(const b2AABB& aabb);

        static b2AABB ScaleUp(const b2AABB& aabb);

        static void b2LinearFrequency(float& frequency, float& ratio, float stiffness,
                                      float damping, b2Body* bodyA, b2Body* bodyB);

        static void b2AngularFrequency(float& frequency, float& ratio, float stiffness,
                                       float damping, b2Body* bodyA, b2Body* bodyB);

        /* lua stuff */

        int GetDistance(lua_State* L);

        World* NewWorld(float gx, float gy, bool sleep);

        Body* NewBody(World* world, float x, float y, Body::Type type);

        Body* NewBody(World* world, Body::Type type);

        Fixture* NewFixture(Body* body, Shape* shape, float density);

        CircleShape* NewCircleShape(float radius);

        CircleShape* NewCircleShape(float x, float y, float radius);

        PolygonShape* NewRectangleShape(float width, float height);

        PolygonShape* NewRectangleShape(float x, float y, float width, float height);

        PolygonShape* NewRectangleShape(float x, float y, float width, float height, float angle);

        EdgeShape* NewEdgeShape(float x1, float y1, float x2, float y2, bool oneSided);

        int NewPolygonShape(lua_State* L);

        int NewChainShape(lua_State* L);

        DistanceJoint* NewDistanceJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                                        bool collideConnected);

        MouseJoint* NewMouseJoint(Body* body, float x, float y);

        RevoluteJoint* NewRevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                        bool collideConnected);

        RevoluteJoint* NewRevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                        bool collideConnected, float referenceAngle);

        PrismaticJoint* NewPrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                          float xAnchor, float yAnchor, bool collideConnected);

        PrismaticJoint* NewPrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                          float xAnchor, float yAnchor, bool collideConnected,
                                          float referenceAngle);

        PulleyJoint* NewPulleyJoint(Body* a, Body* b, b2Vec2 groundAnchorA, b2Vec2 groundAnchorB,
                                    b2Vec2 anchorA, b2Vec2 anchorB, float ratio,
                                    bool collideConnected);

        GearJoint* NewGearJoint(Joint* a, Joint* b, float ratio, bool collideConnected);

        FrictionJoint* NewFrictionJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                        bool collideConnected);

        WeldJoint* NewWeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                bool collideConnected);

        WeldJoint* NewWeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                bool collideConnected, float referenceAngle);

        WheelJoint* NewWheelJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                                  float ax, float ay, bool collideConnected);

        RopeJoint* NewRopeJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                                float maxLength, bool collideConnected);

        MotorJoint* NewMotorJoint(Body* a, Body* b);

        MotorJoint* NewMotorJoint(Body* a, Body* b, float correctionFactor, bool collideConnected);

      private:
        static float meter;
    };
} // namespace love
