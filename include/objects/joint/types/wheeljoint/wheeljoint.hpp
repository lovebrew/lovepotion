#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class WheelJoint : public Joint
    {
      public:
        static love::Type type;

        WheelJoint(Body* a, Body* b, float xA, float yA, float xB, float yB, float aX, float aY,
                   bool collideConnected);

        virtual ~WheelJoint();

        float GetJointTranslation() const;

        float GetJointSpeed() const;

        void SetMotorEnabled(bool enable);

        bool IsMotorEnabled() const;

        void SetMotorSpeed(float speed);

        float GetMotorSpeed() const;

        void SetMaxMotorTorque(float torque);

        float GetMaxMotorTorque() const;

        float GetMotorTorque(float dt) const;

        void SetStiffness(float k);

        float GetStiffness() const;

        void SetDamping(float damping);

        float GetDamping() const;

        int GetAxis(lua_State* L);

      private:
        b2WheelJoint* joint;
    };
} // namespace love
