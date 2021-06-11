#pragma once

#include "joint/joint.h"

namespace love
{
    class RopeJoint : public Joint
    {
      public:
        static love::Type type;

        RopeJoint(Body* a, Body* b, float x1, float y1, float x2, float y2, float maxLength,
                  bool collideConnected);

        virtual ~RopeJoint();

        float GetMaxLength() const;

        void SetMaxLength(float length);

      private:
        b2DistanceJoint* joint;
    };
} // namespace love
