#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class RevoluteJoint : public Joint
    {
      public:
        static love::Type type;

        RevoluteJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB,
                      bool collideConnected);

        RevoluteJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB,
                      bool collideConnected, float referenceAngle);

        virtual ~RevoluteJoint();

        float GetJointAngle() const;

        float GetJointSpeed() const;

        void SetMotorEnabled(bool enable);

        bool IsMotorEnabled() const;

        void SetMaxMotorTorque(float torque);

        void SetMotorSpeed(float speed);

        float GetMotorSpeed() const;

        float GetMotorTorque(float delta) const;

        float GetMaxMotorTorque() const;

        void SetLimitsEnabled(bool enable);

        bool AreLimitsEnabled() const;

        void SetUpperLimit(float limit);

        void SetLowerLimit(float limit);

        void SetLimits(float lower, float upper);

        float GetUpperLimit() const;

        float GetLowerLimit() const;

        int GetLimits(lua_State* L);

        float GetReferenceAngle() const;

      private:
        b2RevoluteJoint* joint;

        void Init(b2RevoluteJointDef& definition, Body* bodyA, Body* bodyB, float xA, float yA,
                  float xB, float yB, bool collideConnected);
    };
} // namespace love
