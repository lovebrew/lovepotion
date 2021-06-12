#pragma once

#include "joint/joint.h"

namespace love
{
    class RevoluteJoint : public Joint
    {
      public:
        static love::Type type;

        RevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                      bool collideConnected);

        RevoluteJoint(Body* a, Body* b, float xA, float yA, float xB, float yB,
                      bool collideConnected, float referenceAngle);

        virtual ~RevoluteJoint();

        float GetJointAngle() const;

        float GetJointSpeed() const;

        void SetMotorEnabled(bool enable);

        bool IsMotorEnabled() const;

        void SetMaxMotorTorque(float torque);

        void SetMotorSpeed(float speed);

        float GetMotorSpeed() const;

        float GetMotorTorque(float invDt) const;

        float GetMaxMotorTorque() const;

        void SetLimitsEnabled(bool enable);

        bool AreLimitsEnabled() const;

        void SetUpperLimit(float limit);

        float GetUpperLimit() const;

        void SetLowerLimit(float limit);

        float GetLowerLimit() const;

        void SetLimits(float lower, float upeper);

        int GetLimits(lua_State* L);

        float GetReferenceAngle() const;

      private:
        b2RevoluteJoint* joint;

        void Initialize(b2RevoluteJointDef& definitio, Body* a, Body* b, float xA, float yA,
                        float xB, float yB, bool collideConnected);
    };
} // namespace love
