#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class PulleyJoint : public Joint
    {
      public:
        static love::Type type;

        PulleyJoint(Body* bodyA, Body* bodyB, b2Vec2 groundAnchorA, b2Vec2 groundAnchorB,
                    b2Vec2 anchorA, b2Vec2 anchorB, float ratio, bool collideConnected);

        virtual ~PulleyJoint();

        int GetGroundAnchors(lua_State* L);

        float GetLengthA() const;

        float GetLengthB() const;

        float GetRatio() const;

      private:
        b2PulleyJoint* joint;
    };
} // namespace love
