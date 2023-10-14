#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class FrictionJoint : public Joint
    {
      public:
        static love::Type type;

        FrictionJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
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
