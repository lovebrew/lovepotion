#pragma once

#include <common/module.hpp>
#include <common/vector.hpp>

#include <objects/body/body.hpp>
#include <objects/contact/contact.hpp>
#include <objects/joint/joint.hpp>
#include <objects/shape/shape.hpp>
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
