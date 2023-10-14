#pragma once

#include <objects/joint/joint.hpp>

namespace love
{
    class DistanceJoint : public Joint
    {
      public:
        static love::Type type;

        DistanceJoint(Body* a, Body* b, float x1, float y1, float x2, float y2,
                      bool collideConnected);

        virtual ~DistanceJoint();

        void SetLength(float length);

        float GetLength() const;

        void SetStiffness(float k);

        float GetStiffness() const;

        void SetDamping(float ratio);

        float GetDamping() const;

      private:
        b2DistanceJoint* joint;
    };
} // namespace love
