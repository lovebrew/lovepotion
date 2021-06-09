#pragma once

#include "common/module.h"
#include "objects/box2d/common.h"

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

      private:
        static float meter;
    };
} // namespace love
