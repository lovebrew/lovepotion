#pragma once

#include "joint/joint.h"

namespace love
{
    class GearJoint : public Joint
    {
      public:
        static love::Type type;

        GearJoint(Joint* a, Joint* b, float ratio, bool collideConnected);

        virtual ~GearJoint();

        void SetRatio(float ratio);

        float GetRatio() const;

        Joint* GetJointA() const;

        Joint* GetJointB() const;

      private:
        b2GearJoint* joint;
    };
} // namespace love
