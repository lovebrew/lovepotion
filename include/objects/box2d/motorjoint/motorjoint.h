#pragma once

#include "joint/joint.h"

namespace love
{
    class MotorJoint : public Joint
    {
      public:
        static love::Type type;

        MotorJoint(Body* a, Body* b);

        MotorJoint(Body* a, Body* b, float correctionFactor, bool collideConnected);

        virtual ~MotorJoint();

        void SetLinearOffset(float x, float y);

        int GetLinearOffset(lua_State* L) const;

        void SetAngularOffset(float offset);

        float GetAngularOffset() const;

        void SetMaxForce(float force);

        float GetMaxForce() const;

        void SetMaxTorque(float torque);

        float GetMaxTorque() const;

        void SetCorrectionFactor(float factor);

        float GetCorrectionFactor() const;

      private:
        b2MotorJoint* joint;
    };
} // namespace love
