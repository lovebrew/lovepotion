#pragma once

#include "joint/joint.h"

namespace love
{
    class WeldJoint : public Joint
    {
      public:
        static love::Type type;

        WeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB, bool collideConnected);

        WeldJoint(Body* a, Body* b, float xA, float yA, float xB, float yB, bool collideConnected,
                  float referenceAngle);

        virtual ~WeldJoint();

        void SetFrequency(float hz);

        float GetFrequency() const;

        void SetDampingRatio(float ratio);

        float GetDampingRatio() const;

        void SetStiffness(float stiffness);

        float GetStiffness() const;

        void SetDamping(float damping);

        float GetDamping() const;

        float GetReferenceAngle() const;

      private:
        b2WeldJoint* joint;

        void Initialize(b2WeldJointDef& definition, Body* a, Body* b, float xA, float yA, float xB,
                        float yB, bool collideConnected);
    };
} // namespace love
