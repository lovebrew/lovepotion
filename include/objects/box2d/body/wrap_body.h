#pragma once

#include "common/luax.h"

#include "body/body.h"

namespace Wrap_Body
{
    int GetX(lua_State* L);

    int GetY(lua_State* L);

    int GetAngle(lua_State* L);

    int GetPosition(lua_State* L);

    int GetTransform(lua_State* L);

    int GetLinearVelocity(lua_State* L);

    int GetWorldCenter(lua_State* L);

    int GetLocalCenter(lua_State* L);

    int GetAngularVelocity(lua_State* L);

    int GetKinematicState(lua_State* L);

    int GetMass(lua_State* L);

    int GetInertia(lua_State* L);

    int GetMassData(lua_State* L);

    int GetAngularDamping(lua_State* L);

    int GetLinearDamping(lua_State* L);

    int GetGravityScale(lua_State* L);

    int GetType(lua_State* L);

    int ApplyLinearImpulse(lua_State* L);

    int ApplyAngularImpulse(lua_State* L);

    int ApplyTorque(lua_State* L);

    int ApplyForce(lua_State* L);

    int SetX(lua_State* L);

    int SetY(lua_State* L);

    int SetTransform(lua_State* L);

    int SetLinearVelocity(lua_State* L);

    int SetAngle(lua_State* L);

    int SetAngularVelocity(lua_State* L);

    int SetPosition(lua_State* L);

    int SetKinematicState(lua_State* L);

    int ResetMassData(lua_State* L);

    int SetMassData(lua_State* L);

    int SetMass(lua_State* L);

    int SetInertia(lua_State* L);

    int SetAngularDamping(lua_State* L);

    int SetLinearDamping(lua_State* L);

    int SetGravityScale(lua_State* L);

    int SetType(lua_State* L);

    int GetWorldPoint(lua_State* L);

    int GetWorldVector(lua_State* L);

    int GetWorldPoints(lua_State* L);

    int GetLocalPoint(lua_State* L);

    int GetLocalVector(lua_State* L);

    int GetLocalPoints(lua_State* L);

    int GetLinearVelocityFromWorldPoint(lua_State* L);

    int GetLinearVelocityFromLocalPoint(lua_State* L);

    int IsBullet(lua_State* L);

    int SetBullet(lua_State* L);

    int IsActive(lua_State* L);

    int IsAwake(lua_State* L);

    int SetSleepingAllowed(lua_State* L);

    int IsSleepingAllowed(lua_State* L);

    int SetActive(lua_State* L);

    int SetAwake(lua_State* L);

    int SetFixedRotation(lua_State* L);

    int IsFixedRotation(lua_State* L);

    int IsTouching(lua_State* L);

    int GetWorld(lua_State* L);

    int GetFixtures(lua_State* L);

    int GetJoints(lua_State* L);

    int GetContacts(lua_State* L);

    int Destroy(lua_State* L);

    int IsDestroyed(lua_State* L);

    int SetUserdata(lua_State* L);

    int GetUserdata(lua_State* L);

    love::Body* CheckBody(lua_State* L, int index);

    int Register(lua_State* L);
} // namespace Wrap_Body
