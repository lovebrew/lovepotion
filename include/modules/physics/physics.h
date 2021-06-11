#pragma once

#include "common/exception.h"
#include "common/module.h"

#include "body.h"
#include "wrap_body.h"

#include "wrap_fixture.h"

#include "chainshape/chainshape.h"
#include "circleshape/circleshape.h"
#include "edgeshape/edgeshape.h"
#include "polygonshape/polygonshape.h"

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

      private:
        static float meter;
    };
} // namespace love
