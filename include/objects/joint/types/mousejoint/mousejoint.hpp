#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class MouseJoint : public Joint
    {
      public:
        static love::Type type;

        MouseJoint(Body* body, float x, float y);

        virtual ~MouseJoint();

        void SetTarget(float x, float y);

        int GetTarget(lua_State* L);

        void SetMaxForce(float force);

        float GetMaxForce() const;

        void SetFrequency(float hz);

        float GetFrequency() const;

        void SetDampingRatio(float ratio);

        float GetDampingRatio() const;

        void SetStiffness(float stiffness);

        float GetStiffness() const;

        void SetDamping(float damping);

        float GetDamping() const;

        virtual Body* GetBodyA() const;

        virtual Body* GetBodyB() const;

      private:
        b2MouseJoint* joint;
    };
} // namespace love
