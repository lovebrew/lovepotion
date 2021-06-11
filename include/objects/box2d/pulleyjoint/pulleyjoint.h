#pragma once

#include "joint/joint.h"

namespace love
{
    class PulleyJoint : public Joint
    {
      public:
        static love::Type type;

        PulleyJoint(Body* a, Body* b, b2Vec2 groundAnchorA, b2Vec2 groundAnchorB, b2Vec2 anchorA,
                    b2Vec2 anchorB, float ratio, bool collideConnected);

        virtual ~PulleyJoint();

        int GetGroundAnchors(lua_State* L);

        float GetLengthA() const;

        float GetLengthB() const;

        float GetRatio() const;

      private:
        b2PulleyJoint* joint;
    };
} // namespace love
