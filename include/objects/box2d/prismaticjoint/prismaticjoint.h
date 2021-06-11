#pragma once

#include "joint/joint.h"

namespace love
{
    class PrismaticJoint : public Joint
    {
      public:
        static love::Type type;

        PrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB, float xAnchor,
                       float yAnchor, bool collideConnected);

        PrismaticJoint(Body* a, Body* b, float xA, float yA, float xB, float yB, float xAnchor,
                       float yAnchor, bool collideConnected, float referenceAngle);

        virtual ~PrismaticJoint();

        float GetJointTranslation() const;

        float GetJointSpeed() const;

        void SetMotorEnabled(bool enable);

        bool IsMotorEnabled() const;

        void SetMaxMotorForce(float force);

        float GetMaxMotorForce() const;

        float GetMotorForce(float invDt) const;

        void SetMotorSpeed(float speed);

        float GetMotorSpeed() const;

        void SetLimitsEnabled(bool enable);

        bool AreLimitsEnabled() const;

        void SetUpperLimit(float limit);

        float GetUpperLimit() const;

        void SetLowerLimit(float limit);

        float GetLowerLimit() const;

        void SetLimits(float lower, float upeper);

        int GetLimits(lua_State* L);

        int GetAxis(lua_State* L);

        float GetReferenceAngle() const;

      private:
        b2PrismaticJoint* joint;

        void Initialize(b2PrismaticJointDef& def, Body* body1, Body* body2, float xA, float yA,
                        float xB, float yB, float ax, float ay, bool collideConnected);
    };
} // namespace love
