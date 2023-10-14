#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class RopeJoint : public Joint
    {
      public:
        static love::Type type;

        RopeJoint(Body* bodyA, Body* bodyB, float x1, float y1, float x2, float y2, float maxLength,
                  bool collideConnected);

        virtual ~RopeJoint();

        float GetMaxLength() const;

        void SetMaxLength(float maxLength);

      private:
        b2DistanceJoint* joint;
    };
} // namespace love
