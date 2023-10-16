#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class PrismaticJoint : public Joint
    {
      public:
        static love::Type type;

        PrismaticJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB, float aX,
                       float aY, bool collideConnected);

        PrismaticJoint(Body* bodyA, Body* bodyB, float xA, float yA, float xB, float yB, float aX,
                       float aY, bool collideConnected, float referenceAngle);

        virtual ~PrismaticJoint();

        float GetJointTranslation() const;

        float GetJointSpeed() const;

        void SetMotorEnabled(bool enable);

        bool IsMotorEnabled() const;

        void SetMaxMotorForce(float force);

        void SetMotorSpeed(float speed);

        float GetMotorSpeed() const;

        float GetMotorForce(float inverseDelta) const;

        float GetMaxMotorForce() const;

        void SetLimitsEnabled(bool enable);

        bool AreLimitsEnabled() const;

        void SetUpperLimit(float limit);

        void SetLowerLimit(float limit);

        void SetLimits(float lower, float upper);

        float GetUpperLimit() const;

        float GetLowerLimit() const;

        int GetLimits(lua_State* L);

        int GetAxis(lua_State* L);

        float GetReferenceAngle() const;

      private:
        b2PrismaticJoint* joint;

        void Init(b2PrismaticJointDef& definition, Body* bodyA, Body* bodyB, float xA, float yA,
                  float xB, float yB, float aX, float aY, bool collideConnected);
    };
} // namespace love
