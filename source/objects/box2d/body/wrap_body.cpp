#include "objects/box2d/body/wrap_body.h"
#include "modules/physics/wrap_physics.h"

using namespace love;

Body* Wrap_Body::CheckBody(lua_State* L, int index)
{
    Body* body = Luax::CheckType<Body>(L, index);

    if (body->body == 0)
        luaL_error(L, "Attempt to use destroyed body!");

    return body;
}

int Wrap_Body::GetX(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetX());

    return 1;
}

int Wrap_Body::GetY(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetY());

    return 1;
}

int Wrap_Body::GetPosition(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float xOut, yOut;
    self->GetPosition(xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetAngle(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetAngle());

    return 1;
}

int Wrap_Body::GetTransform(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float xOut, yOut;
    self->GetPosition(xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);
    lua_pushnumber(L, self->GetAngle());

    return 3;
}

int Wrap_Body::GetLinearVelocity(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float xOut, yOut;
    self->GetLinearVelocity(xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetWorldCenter(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float xOut, yOut;
    self->GetWorldCenter(xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetLocalCenter(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float xOut, yOut;
    self->GetLocalCenter(xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetAngularVelocity(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetAngularVelocity());

    return 1;
}

int Wrap_Body::GetKinematicState(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    b2Vec2 positionOut, velocityOut;
    float angleOut, angularVelocityOut;

    self->GetKinematicState(positionOut, angleOut, velocityOut, angularVelocityOut);

    lua_pushnumber(L, positionOut.x);
    lua_pushnumber(L, positionOut.y);
    lua_pushnumber(L, angleOut);
    lua_pushnumber(L, velocityOut.x);
    lua_pushnumber(L, velocityOut.y);
    lua_pushnumber(L, angularVelocityOut);

    return 6;
}

int Wrap_Body::GetMass(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetMass());

    return 1;
}

int Wrap_Body::GetInertia(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetInertia());

    return 1;
}

int Wrap_Body::GetMassData(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetMassData(L);
}

int Wrap_Body::GetAngularDamping(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetAngularDamping());

    return 1;
}

int Wrap_Body::GetLinearDamping(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetLinearDamping());

    return 1;
}

int Wrap_Body::GetGravityScale(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetGravityScale());

    return 1;
}

int Wrap_Body::GetType(lua_State* L)
{
    Body* self       = Wrap_Body::CheckBody(L, 1);
    const char* type = "";

    Body::GetConstant(self->GetType(), type);

    lua_pushstring(L, type);

    return 1;
}

int Wrap_Body::ApplyLinearImpulse(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float jx = luaL_checknumber(L, 2);
    float jy = luaL_checknumber(L, 3);

    int nargs = lua_gettop(L);

    if (nargs <= 3 || (nargs == 4 && lua_type(L, 4) == LUA_TBOOLEAN))
    {
        bool awake = Luax::OptBoolean(L, 4, true);
        self->ApplyLinearImpulse(jx, jy, awake);
    }
    else if (nargs >= 5)
    {
        float rx = luaL_checknumber(L, 4);
        float ry = luaL_checknumber(L, 5);

        bool awake = Luax::OptBoolean(L, 6, true);
        self->ApplyLinearImpulse(jx, jy, rx, ry, awake);
    }
    else
        return luaL_error(L, "Wrong number of parameters.");

    return 0;
}

int Wrap_Body::ApplyAngularImpulse(lua_State* L)
{
    Body* self    = Wrap_Body::CheckBody(L, 1);
    float impulse = luaL_checknumber(L, 2);

    bool awake = Luax::OptBoolean(L, 3, true);

    self->ApplyAngularImpulse(impulse, awake);

    return 0;
}

int Wrap_Body::ApplyTorque(lua_State* L)
{
    Body* self   = Wrap_Body::CheckBody(L, 1);
    float torque = luaL_checknumber(L, 2);

    bool awake = Luax::OptBoolean(L, 3, true);

    self->ApplyTorque(torque, awake);

    return 0;
}

int Wrap_Body::ApplyForce(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float fx = luaL_checknumber(L, 2);
    float fy = luaL_checknumber(L, 3);

    int nargs = lua_gettop(L);

    if (nargs <= 3 || (nargs == 4 && lua_type(L, 4) == LUA_TBOOLEAN))
    {
        bool awake = Luax::OptBoolean(L, 4, true);

        self->ApplyForce(fx, fy, awake);
    }
    else if (lua_gettop(L) >= 5)
    {
        float rx = (float)luaL_checknumber(L, 4);
        float ry = (float)luaL_checknumber(L, 5);

        bool awake = Luax::OptBoolean(L, 6, true);

        self->ApplyForce(fx, fy, rx, ry, awake);
    }
    else
        return luaL_error(L, "Wrong number of parameters.");

    return 0;
}

int Wrap_Body::SetX(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    float x    = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetX(x); });

    return 0;
}

int Wrap_Body::SetY(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    float y    = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetY(y); });

    return 0;
}

int Wrap_Body::SetTransform(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float angle = luaL_checknumber(L, 4);

    Luax::CatchException(L, [&]() {
        self->SetPosition(x, y);
        self->SetAngle(angle);
    });

    return 0;
}

int Wrap_Body::SetLinearVelocity(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->SetLinearVelocity(x, y);

    return 0;
}

int Wrap_Body::SetAngle(lua_State* L)
{
    Body* self  = Wrap_Body::CheckBody(L, 1);
    float angle = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetAngle(angle); });

    return 0;
}

int Wrap_Body::SetAngularVelocity(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    float spin = luaL_checknumber(L, 2);

    self->SetAngularVelocity(spin);

    return 0;
}

int Wrap_Body::SetPosition(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    Luax::CatchException(L, [&]() { self->SetPosition(x, y); });

    return 0;
}

int Wrap_Body::SetKinematicState(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x     = luaL_checknumber(L, 2);
    float y     = luaL_checknumber(L, 3);
    float angle = luaL_checknumber(L, 4);
    float dx    = luaL_checknumber(L, 5);
    float dy    = luaL_checknumber(L, 6);
    float da    = luaL_checknumber(L, 7);

    Luax::CatchException(
        L, [&]() { self->SetKinematicState(b2Vec2(x, y), angle, b2Vec2(dx, dy), da); });

    return 0;
}

int Wrap_Body::ResetMassData(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    Luax::CatchException(L, [&]() { self->ResetMassData(); });

    return 0;
}

int Wrap_Body::SetMassData(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x       = luaL_checknumber(L, 2);
    float y       = luaL_checknumber(L, 3);
    float mass    = luaL_checknumber(L, 4);
    float inertia = luaL_checknumber(L, 5);

    Luax::CatchException(L, [&]() { self->SetMassData(x, y, mass, inertia); });

    return 0;
}

int Wrap_Body::SetMass(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    float mass = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetMass(mass); });

    return 0;
}

int Wrap_Body::SetInertia(lua_State* L)
{
    Body* self    = Wrap_Body::CheckBody(L, 1);
    float inertia = luaL_checknumber(L, 2);

    Luax::CatchException(L, [&]() { self->SetInertia(inertia); });

    return 0;
}

int Wrap_Body::SetAngularDamping(lua_State* L)
{
    Body* self           = Wrap_Body::CheckBody(L, 1);
    float angularDamping = luaL_checknumber(L, 2);

    self->SetAngularDamping(angularDamping);

    return 0;
}

int Wrap_Body::SetLinearDamping(lua_State* L)
{
    Body* self          = Wrap_Body::CheckBody(L, 1);
    float linearDamping = luaL_checknumber(L, 2);

    self->SetLinearDamping(linearDamping);

    return 0;
}

int Wrap_Body::SetGravityScale(lua_State* L)
{
    Body* self  = Wrap_Body::CheckBody(L, 1);
    float scale = luaL_checknumber(L, 2);

    self->SetGravityScale(scale);

    return 0;
}

int Wrap_Body::SetType(lua_State* L)
{
    Body* self          = Wrap_Body::CheckBody(L, 1);
    const char* typeStr = luaL_checkstring(L, 2);

    Body::Type type;
    if (!Body::GetConstant(typeStr, type))
        return Luax::EnumError(L, "body type", Body::GetConstants(type), typeStr);

    Luax::CatchException(L, [&]() { self->SetType(type); });

    return 0;
}

int Wrap_Body::GetWorldPoint(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float xOut, yOut;
    self->GetWorldPoint(x, y, xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetWorldVector(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float xOut, yOut;
    self->GetWorldVector(x, y, xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetWorldPoints(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetWorldPoints(L);
}

int Wrap_Body::GetLocalPoint(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float xOut, yOut;
    self->GetLocalPoint(x, y, xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetLocalVector(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float xOut, yOut;
    self->GetLocalVector(x, y, xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetLocalPoints(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetLocalPoints(L);
}

int Wrap_Body::GetLinearVelocityFromWorldPoint(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float xOut, yOut;
    self->GetLinearVelocityFromWorldPoint(x, y, xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::GetLinearVelocityFromLocalPoint(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float xOut, yOut;
    self->GetLinearVelocityFromLocalPoint(x, y, xOut, yOut);

    lua_pushnumber(L, xOut);
    lua_pushnumber(L, yOut);

    return 2;
}

int Wrap_Body::IsBullet(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    Luax::PushBoolean(L, self->IsBullet());

    return 1;
}

int Wrap_Body::SetBullet(lua_State* L)
{
    Body* self  = Wrap_Body::CheckBody(L, 1);
    bool bullet = Luax::CheckBoolean(L, 2);

    self->SetBullet(bullet);

    return 0;
}

int Wrap_Body::IsActive(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    Luax::PushBoolean(L, self->IsEnabled());

    return 1;
}

int Wrap_Body::IsAwake(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    Luax::PushBoolean(L, self->IsAwake());

    return 1;
}

int Wrap_Body::SetSleepingAllowed(lua_State* L)
{
    Body* self   = Wrap_Body::CheckBody(L, 1);
    bool allowed = Luax::CheckBoolean(L, 2);

    self->SetSleepingAllowed(allowed);

    return 0;
}

int Wrap_Body::IsSleepingAllowed(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    Luax::PushBoolean(L, self->IsSleepingAllowed());

    return 1;
}

int Wrap_Body::SetActive(lua_State* L)
{
    Body* self  = Wrap_Body::CheckBody(L, 1);
    bool active = Luax::CheckBoolean(L, 2);

    Luax::CatchException(L, [&]() { self->SetActive(active); });

    return 0;
}

int Wrap_Body::SetAwake(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    bool awake = Luax::CheckBoolean(L, 2);

    self->SetAwake(awake);

    return 0;
}

int Wrap_Body::SetFixedRotation(lua_State* L)
{
    Body* self         = Wrap_Body::CheckBody(L, 1);
    bool fixedRotation = Luax::CheckBoolean(L, 2);

    Luax::CatchException(L, [&]() { self->SetFixedRotation(fixedRotation); });

    return 0;
}

int Wrap_Body::IsFixedRotation(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    Luax::PushBoolean(L, self->IsFixedRotation());

    return 1;
}

int Wrap_Body::IsTouching(lua_State* L)
{
    Body* self  = Wrap_Body::CheckBody(L, 1);
    Body* other = Wrap_Body::CheckBody(L, 2);

    Luax::PushBoolean(L, self->IsTouching(other));

    return 1;
}

int Wrap_Body::GetWorld(lua_State* L)
{
    Body* self   = Wrap_Body::CheckBody(L, 1);
    World* world = self->GetWorld();

    Luax::PushType(L, world);

    return 1;
}

int Wrap_Body::GetFixtures(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    int n = 0;

    Luax::CatchException(L, [&]() { n = self->GetFixtures(L); });

    return n;
}

int Wrap_Body::GetJoints(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    int n = 0;

    Luax::CatchException(L, [&]() { n = self->GetJoints(L); });

    return n;
}

int Wrap_Body::GetContacts(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    int n = 0;

    Luax::CatchException(L, [&]() { n = self->GetContacts(L); });

    return n;
}

int Wrap_Body::Destroy(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);

    Luax::CatchException(L, [&]() { self->Destroy(); });

    return 0;
}

int Wrap_Body::IsDestroyed(lua_State* L)
{
    Body* self = Luax::CheckType<Body>(L, 1);

    Luax::PushBoolean(L, self->body == nullptr);

    return 1;
}

int Wrap_Body::SetUserdata(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->SetUserData(L);
}

int Wrap_Body::GetUserdata(lua_State* L)
{
    Body* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetUserData(L);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "applyAngularImpulse",             Wrap_Body::ApplyAngularImpulse             },
    { "applyForce",                      Wrap_Body::ApplyForce                      },
    { "applyLinearImpulse",              Wrap_Body::ApplyLinearImpulse              },
    { "applyTorque",                     Wrap_Body::ApplyTorque                     },
    { "destroy",                         Wrap_Body::Destroy                         },
    { "getAngle",                        Wrap_Body::GetAngle                        },
    { "getAngularDamping",               Wrap_Body::GetAngularDamping               },
    { "getAngularVelocity",              Wrap_Body::GetAngularVelocity              },
    { "getContacts",                     Wrap_Body::GetContacts                     },
    { "getFixtures",                     Wrap_Body::GetFixtures                     },
    { "getGravityScale",                 Wrap_Body::GetGravityScale                 },
    { "getInertia",                      Wrap_Body::GetInertia                      },
    { "getJoints",                       Wrap_Body::GetJoints                       },
    { "getKinematicState",               Wrap_Body::GetKinematicState               },
    { "getLinearDamping",                Wrap_Body::GetLinearDamping                },
    { "getLinearVelocity",               Wrap_Body::GetLinearVelocity               },
    { "getLinearVelocityFromLocalPoint", Wrap_Body::GetLinearVelocityFromLocalPoint },
    { "getLinearVelocityFromWorldPoint", Wrap_Body::GetLinearVelocityFromWorldPoint },
    { "getLocalCenter",                  Wrap_Body::GetLocalCenter                  },
    { "getLocalPoint",                   Wrap_Body::GetLocalPoint                   },
    { "getLocalPoints",                  Wrap_Body::GetLocalPoints                  },
    { "getLocalVector",                  Wrap_Body::GetLocalVector                  },
    { "getMass",                         Wrap_Body::GetMass                         },
    { "getMassData",                     Wrap_Body::GetMassData                     },
    { "getPosition",                     Wrap_Body::GetPosition                     },
    { "getTransform",                    Wrap_Body::GetTransform                    },
    { "getType",                         Wrap_Body::GetType                         },
    { "getUserData",                     Wrap_Body::GetUserdata                     },
    { "getWorld",                        Wrap_Body::GetWorld                        },
    { "getWorldCenter",                  Wrap_Body::GetWorldCenter                  },
    { "getWorldPoint",                   Wrap_Body::GetWorldPoint                   },
    { "getWorldPoints",                  Wrap_Body::GetWorldPoints                  },
    { "getWorldVector",                  Wrap_Body::GetWorldVector                  },
    { "getX",                            Wrap_Body::GetX                            },
    { "getY",                            Wrap_Body::GetY                            },
    { "isActive",                        Wrap_Body::IsActive                        },
    { "isAwake",                         Wrap_Body::IsAwake                         },
    { "isBullet",                        Wrap_Body::IsBullet                        },
    { "isDestroyed",                     Wrap_Body::IsDestroyed                     },
    { "isFixedRotation",                 Wrap_Body::IsFixedRotation                 },
    { "isSleepingAllowed",               Wrap_Body::IsSleepingAllowed               },
    { "isTouching",                      Wrap_Body::IsTouching                      },
    { "resetMassData",                   Wrap_Body::ResetMassData                   },
    { "setActive",                       Wrap_Body::SetActive                       },
    { "setAngle",                        Wrap_Body::SetAngle                        },
    { "setAngularDamping",               Wrap_Body::SetAngularDamping               },
    { "setAngularVelocity",              Wrap_Body::SetAngularVelocity              },
    { "setAwake",                        Wrap_Body::SetAwake                        },
    { "setBullet",                       Wrap_Body::SetBullet                       },
    { "setFixedRotation",                Wrap_Body::SetFixedRotation                },
    { "setGravityScale",                 Wrap_Body::SetGravityScale                 },
    { "setInertia",                      Wrap_Body::SetInertia                      },
    { "setKinematicState",               Wrap_Body::SetKinematicState               },
    { "setLinearDamping",                Wrap_Body::SetLinearDamping                },
    { "setLinearVelocity",               Wrap_Body::SetLinearVelocity               },
    { "setMass",                         Wrap_Body::SetMass                         },
    { "setMassData",                     Wrap_Body::SetMassData                     },
    { "setPosition",                     Wrap_Body::SetPosition                     },
    { "setSleepingAllowed",              Wrap_Body::SetSleepingAllowed              },
    { "setTransform",                    Wrap_Body::SetTransform                    },
    { "setType",                         Wrap_Body::SetType                         },
    { "setUserData",                     Wrap_Body::SetUserdata                     },
    { "setX",                            Wrap_Body::SetX                            },
    { "setY",                            Wrap_Body::SetY                            },
    { 0,                                 0                                          }
};
// clang-format on

int Wrap_Body::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Body::type, functions, nullptr);
}
