#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class GearJoint : public Joint
    {
      public:
        static love::Type type;

        GearJoint(Joint* joint1, Joint* joint2, float ratio, bool collideConnected);

        virtual ~GearJoint();

        void SetRatio(float ratio);

        float GetRatio() const;

        Joint* GetJointA() const;

        Joint* GetJointB() const;

      private:
        b2GearJoint* joint;
    };
} // namespace love
