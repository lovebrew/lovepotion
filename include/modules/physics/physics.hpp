#pragma once

#include <common/module.hpp>
#include <common/vector.hpp>

#include <objects/body/body.hpp>
#include <objects/contact/contact.hpp>

#include <objects/joint/joint.hpp>
#include <objects/joint/types/distancejoint/distancejoint.hpp>
#include <objects/joint/types/frictionjoint/frictionjoint.hpp>
#include <objects/joint/types/gearjoint/gearjoint.hpp>
#include <objects/joint/types/motorjoint/motorjoint.hpp>
#include <objects/joint/types/mousejoint/mousejoint.hpp>
#include <objects/joint/types/prismaticjoint/prismaticjoint.hpp>
#include <objects/joint/types/pulleyjoint/pulleyjoint.hpp>
#include <objects/joint/types/revolutejoint/revolutejoint.hpp>
#include <objects/joint/types/ropejoint/ropejoint.hpp>
#include <objects/joint/types/weldjoint/weldjoint.hpp>
#include <objects/joint/types/wheeljoint/wheeljoint.hpp>

#include <objects/shape/shape.hpp>
#include <objects/shape/types/chainshape/chainshape.hpp>
#include <objects/shape/types/circleshape/circleshape.hpp>
#include <objects/shape/types/edgeshape/edgeshape.hpp>
#include <objects/shape/types/polygonshape/polygonshape.hpp>

#include <objects/world/world.hpp>

namespace love
{
    class Physics : public Module
    {
      public:
        static constexpr int DEFAULT_METER = 30;

        Physics();

        virtual ~Physics();

        const char* GetName() const override
        {
            return "love.physics";
        }

        virtual ModuleType GetModuleType() const
        {
            return M_PHYSICS;
        }

        World* NewWorld(float gravityX, float gravityY, bool sleep) const;

        Body* NewBody(World* world, float x, float y, Body::Type type) const;

        Body* NewBody(World* world, Body::Type type) const;

        Body* NewCircleBody(World* world, Body::Type type, float x, float y, float radius) const;

        Body* NewRectangleBody(World* world, Body::Type type, float x, float y, float width,
                               float height, float angle) const;

        Body* NewPolygonBody(World* world, Body::Type type, const std::span<Vector2>& points) const;

        Body* NewEdgeBody(World* world, Body::Type type, float x1, float y1, float x2, float y2,
                          bool oneSided) const;

        Body* NewChainBody(World* world, Body::Type type, bool loop,
                           const std::span<Vector2>& points) const;

        CircleShape* NewCircleShape(Body* body, float x, float y, float radius) const;

        PolygonShape* NewRectangleShape(Body* body, float x, float y, float width, float height,
                                        float angle) const;

        EdgeShape* NewEdgeShape(Body* body, float x1, float y1, float x2, float y2,
                                bool oneSided) const;

        PolygonShape* NewPolygonShape(Body* body, const std::span<Vector2>& points) const;

        ChainShape* NewChainShape(Body* body, bool loop, const std::span<Vector2>& points) const;

        DistanceJoint* NewDistanceJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2,
                                        float y2, bool collideConnected) const;

        FrictionJoint* NewFrictionJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2,
                                        float y2, bool collideConnected) const;

        GearJoint* NewGearJoint(Joint* jointA, Joint* jointB, float ratio,
                                bool collideConnected) const;

        MotorJoint* NewMotorJoint(Body* bodyA, Body* bodyB);

        MotorJoint* NewMotorJoint(Body* bodyA, Body* bodyB, float corrrectionFactor,
                                  bool collideConnected) const;

        MouseJoint* NewMouseJoint(Body* body, float x, float y) const;

        PrismaticJoint* NewPrismaticJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2,
                                          float y2, float axisX, float axisY,
                                          bool collideConnected) const;

        PrismaticJoint* NewPrismaticJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2,
                                          float y2, float axisX, float axisY, bool collideConnected,
                                          float referenceAngle) const;

        PulleyJoint* NewPulleyJoint(Body* bodyA, Body* bodyB, b2Vec2 groundAnchorA,
                                    b2Vec2 groundAnchorB, b2Vec2 anchorA, b2Vec2 anchorB,
                                    float ratio, bool collideConnected) const;

        RevoluteJoint* NewRevoluteJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB,
                                        float yB, bool collideConnected) const;

        RevoluteJoint* NewRevoluteJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB,
                                        float yB, bool collideConnected,
                                        float referenceAngle) const;

        RopeJoint* NewRopeJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2, float y2,
                                float maxLength, bool collideConnected) const;

        WeldJoint* NewWeldJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2, float y2,
                                bool collideConnected) const;

        WeldJoint* NewWeldJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2, float y2,
                                bool collideConnected, float referenceAngle) const;

        WheelJoint* NewWheelJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2, float y2,
                                  float axisX, float axisY, bool collideConnected) const;

        int GetDistance(lua_State* L);

        static void SetMeter(float scale);

        static float GetMeter();

        static float ScaleDown(float value);

        static float ScaleUp(float value);

        static void ScaleDown(float& x, float& y);

        static void ScaleUp(float& x, float& y);

        static b2Vec2 ScaleDown(const b2Vec2& vector);

        static b2Vec2 ScaleUp(const b2Vec2& vector);

        static b2AABB ScaleDown(const b2AABB& aabb);

        static b2AABB ScaleUp(const b2AABB& aabb);

        static void ComputeLinearStiffness(float& stiffness, float& damping, float frequency,
                                           float ratio, const b2Body* bodyA, const b2Body* bodyB);

        static void ComputeLinearFrequency(float& frequency, float& ratio, float stiffness,
                                           float damping, b2Body* bodyA, b2Body* bodyB);

        static void ComputeAngularStiffness(float& frequency, float& ratio, float stiffness,
                                            float damping, const b2Body* bodyA,
                                            const b2Body* bodyB);

        static void ComputeAngularFrequency(float& frequency, float& ratio, float stiffness,
                                            float damping, const b2Body* bodyA,
                                            const b2Body* bodyB);

        b2BlockAllocator* GetAllocator()
        {
            return &this->allocator;
        }

      private:
        static float meter;
        b2BlockAllocator allocator;
    };
} // namespace love
