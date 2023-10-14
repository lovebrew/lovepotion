#pragma once

#include <common/luax.hpp>
#include <common/object.hpp>
#include <common/reference.hpp>

#include <box2d/box2d.h>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    class Body;
    class World;

    class Joint : public Object
    {
      public:
        static love::Type type;

        friend class GearJoint;

        enum Type
        {
            JOINT_INVALID,
            JOINT_DISTANCE,
            JOINT_REVOLUTE,
            JOINT_PRISMATIC,
            JOINT_MOUSE,
            JOINT_PULLEY,
            JOINT_GEAR,
            JOINT_FRICTION,
            JOINT_WELD,
            JOINT_WHEEL,
            JOINT_ROPE,
            JOINT_MOTOR,
            JOINT_MAX_ENUM
        };

        Joint(Body* body);

        Joint(Body* a, Body* b);

        virtual ~Joint();

        bool IsValid() const;

        Joint::Type GetType() const;

        virtual Body* GetBodyA() const;

        virtual Body* GetBodyB() const;

        int GetAnchors(lua_State* L);

        int GetReactionForce(lua_State* L);

        float GetReactionTorque(float delta);

        bool IsEnabled() const;

        bool GetCollideConnected() const;

        int SetUserdata(lua_State* L);

        int GetUserdata(lua_State* L);

        void DestroyJoint(bool implicit = false);

        // clang-format off
        static constexpr BidirectionalMap jointTypes =
        {
            "distance",  JOINT_DISTANCE,
            "revolute",  JOINT_REVOLUTE,
            "prismatic", JOINT_PRISMATIC,
            "mouse",     JOINT_MOUSE,
            "pulley",    JOINT_PULLEY,
            "gear",      JOINT_GEAR,
            "friction",  JOINT_FRICTION,
            "weld",      JOINT_WELD,
            "wheel",     JOINT_WHEEL,
            "rope",      JOINT_ROPE,
            "motor",     JOINT_MOTOR
        };
        // clang-format on

      protected:
        b2Joint* CreateJoint(b2JointDef* def);
        World* world;
        Reference* reference = nullptr;

      private:
        Body* bodyA;
        Body* bodyB;

        b2Joint* joint;
    };
} // namespace love
