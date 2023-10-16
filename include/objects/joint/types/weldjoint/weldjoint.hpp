#pragma once

#include <objects/joint/joint.hpp>

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

        void SetStiffness(float k);

        float GetStiffness() const;

        void SetDamping(float ratio);

        float GetDamping() const;

        float GetReferenceAngle() const;

      private:
        b2WeldJoint* joint;

        void Init(b2WeldJointDef& definition, Body* a, Body* b, float xA, float yA, float xB,
                  float yB, bool collideConnected);
    };
} // namespace love
