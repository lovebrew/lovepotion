#pragma once

#include "joint/joint.h"

namespace love
{
    class FrictionJoint : public Joint
    {
      public:
        static love::Type type;

        FrictionJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                      bool collideConnected);

        virtual ~FrictionJoint();

        void SetMaxForce(float force);

        float GetMaxForce() const;

        void SetMaxTorque(float torque);

        float GetMaxTorque() const;

      private:
        b2FrictionJoint* joint;
    };
} // namespace love
