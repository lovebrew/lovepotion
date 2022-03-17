#pragma once

#include "common/luax.h"

#include "common/reference.h"
#include "objects/object.h"

#include <box2d/box2d.h>

#include <vector>

namespace love
{
    class Body;
    class World;

    class Joint : public Object
    {
      public:
        static love::Type type;

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

        struct JointUserdata
        {
            Reference* ref = nullptr;
        };

        friend class GearJoint;

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

        static bool GetConstant(const char* in, Joint::Type& out);
        static bool GetConstant(Joint::Type in, const char*& out);
        std::vector<const char*> GetConstants(Joint::Type);

      protected:
        b2Joint* CreateJoint(b2JointDef* jointDefinition);
        World* world;
        JointUserdata* userdata;

      private:
        Body* bodyA;
        Body* bodyB;

        b2Joint* joint;
    };
} // namespace love
