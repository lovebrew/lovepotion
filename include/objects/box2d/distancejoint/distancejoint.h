#pragma once

#include "joint/joint.h"

namespace love
{
    class DistanceJoint : public Joint
    {
      public:
        DistanceJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                      bool collideConnected);

        virtual ~DistanceJoint();

        void SetLength(float length);

        float GetLength() const;

        void SetFrequency(float hz);

        float GetFrequency() const;

        void SetDampingRatio(float ratio);

        float GetDampingRatio() const;

        void SetStiffness(float stiffness);

        float GetStiffness() const;

        void SetDamping(float ratio);

        float GetDamping() const;

      private:
        b2DistanceJoint* joint;
    };
} // namespace love
