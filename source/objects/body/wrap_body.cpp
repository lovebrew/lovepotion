#include <objects/body/wrap_body.hpp>

#include <modules/physics/wrap_physics.hpp>

#include <objects/shape/wrap_shape.hpp>

using namespace love;

Body* Wrap_Body::CheckBody(lua_State* L, int index)
{
    auto* body = luax::CheckType<Body>(L, index);

    if (body->body == 0)
        luaL_error(L, "Attempt to use destroyed body.");

    return body;
}

int Wrap_Body::GetX(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetX());

    return 1;
}

int Wrap_Body::GetY(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetY());

    return 1;
}

int Wrap_Body::GetAngle(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetAngle());

    return 1;
}

int Wrap_Body::GetPosition(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x, y = 0.0f;
    self->GetPosition(x, y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
}

int Wrap_Body::GetTransform(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x, y = 0.0f;
    self->GetPosition(x, y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);
    lua_pushnumber(L, self->GetAngle());

    return 3;
}

int Wrap_Body::GetLinearVelocity(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x, y = 0.0f;
    self->GetLinearVelocity(x, y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
}

int Wrap_Body::GetWorldCenter(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x, y = 0.0f;
    self->GetWorldCenter(x, y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
}

int Wrap_Body::GetLocalCenter(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x, y = 0.0f;
    self->GetLocalCenter(x, y);

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    return 2;
}

int Wrap_Body::GetAngularVelocity(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetAngularVelocity());

    return 1;
}

int Wrap_Body::GetKinematicState(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    b2Vec2 position {};
    b2Vec2 velocity {};

    float angle, angularVelocity = 0.0f;
    self->GetKinematicState(position, angle, velocity, angularVelocity);

    lua_pushnumber(L, position.x);
    lua_pushnumber(L, position.y);
    lua_pushnumber(L, angle);
    lua_pushnumber(L, velocity.x);
    lua_pushnumber(L, velocity.y);
    lua_pushnumber(L, angularVelocity);

    return 6;
}

int Wrap_Body::GetMass(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetMass());

    return 1;
}

int Wrap_Body::GetInertia(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetInertia());

    return 1;
}

int Wrap_Body::GetMassData(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetMassData(L);
}

int Wrap_Body::HasCustomMassData(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushboolean(L, self->HasCustomMassData());

    return 1;
}

int Wrap_Body::GetAngularDamping(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetAngularDamping());

    return 1;
}

int Wrap_Body::GetLinearDamping(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetLinearDamping());

    return 1;
}

int Wrap_Body::GetGravityScale(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushnumber(L, self->GetGravityScale());

    return 1;
}

int Wrap_Body::GetType(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    auto type  = Body::bodyTypes.ReverseFind(self->GetType());

    lua_pushstring(L, *type);

    return 1;
}

int Wrap_Body::ApplyLinearImpulse(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    const auto args = lua_gettop(L);

    if (args <= 3 || (args == 4 && lua_type(L, 4) == LUA_TBOOLEAN))
    {
        bool awake = luax::OptBoolean(L, 4, true);
        self->ApplyLinearImpulse(x, y, awake);
    }
    else if (args >= 5)
    {
        float rx   = luaL_checknumber(L, 4);
        float ry   = luaL_checknumber(L, 5);
        bool awake = luax::OptBoolean(L, 6, true);

        self->ApplyLinearImpulse(x, y, rx, ry, awake);
    }
    else
        return luaL_error(L, "Wrong number of parameters.");

    return 0;
}

int Wrap_Body::ApplyAngularImpulse(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float impulse = luaL_checknumber(L, 2);
    bool awake    = luax::OptBoolean(L, 3, true);

    self->ApplyAngularImpulse(impulse, awake);

    return 0;
}

int Wrap_Body::ApplyTorque(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float torque = luaL_checknumber(L, 2);
    bool awake   = luax::OptBoolean(L, 3, true);

    self->ApplyTorque(torque, awake);

    return 0;
}

int Wrap_Body::ApplyForce(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    const auto args = lua_gettop(L);

    if (args <= 3 || (args == 4 && lua_type(L, 4) == LUA_TBOOLEAN))
    {
        bool awake = luax::OptBoolean(L, 4, true);
        self->ApplyForce(x, y, awake);
    }
    else if (lua_gettop(L) >= 5)
    {
        float rx   = luaL_checknumber(L, 4);
        float ry   = luaL_checknumber(L, 5);
        bool awake = luax::OptBoolean(L, 6, true);

        self->ApplyForce(x, y, rx, ry, awake);
    }
    else
        return luaL_error(L, "Wrong number of parameters.");

    return 0;
}

int Wrap_Body::SetX(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetX(x); });

    return 0;
}

int Wrap_Body::SetY(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float y = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetY(y); });

    return 0;
}

int Wrap_Body::SetTransform(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x     = luaL_checknumber(L, 2);
    float y     = luaL_checknumber(L, 3);
    float angle = luaL_checknumber(L, 4);

    luax::CatchException(L, [&]() {
        self->SetPosition(x, y);
        self->SetAngle(angle);
    });

    return 0;
}

int Wrap_Body::SetLinearVelocity(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->SetLinearVelocity(x, y); });

    return 0;
}

int Wrap_Body::SetAngle(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float angle = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetAngle(angle); });

    return 0;
}

int Wrap_Body::SetAngularVelocity(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float omega = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetAngularVelocity(omega); });

    return 0;
}

int Wrap_Body::SetPosition(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    luax::CatchException(L, [&]() { self->SetPosition(x, y); });

    return 0;
}

int Wrap_Body::SetKinematicState(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x     = luaL_checknumber(L, 2);
    float y     = luaL_checknumber(L, 3);
    float angle = luaL_checknumber(L, 4);
    float vx    = luaL_checknumber(L, 5);
    float vy    = luaL_checknumber(L, 6);
    float omega = luaL_checknumber(L, 7);

    luax::CatchException(L, [&]() {
        auto position = b2Vec2(x, y);
        auto velocity = b2Vec2(vx, vy);
        self->SetKinematicState(position, angle, velocity, omega);
    });

    return 0;
}

int Wrap_Body::ResetMassData(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    luax::CatchException(L, [&]() { self->ResetMassData(); });

    return 0;
}

int Wrap_Body::SetMassData(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x       = luaL_checknumber(L, 2);
    float y       = luaL_checknumber(L, 3);
    float mass    = luaL_checknumber(L, 4);
    float inertia = luaL_checknumber(L, 5);

    luax::CatchException(L, [&]() { self->SetMassData(x, y, mass, inertia); });

    return 0;
}

int Wrap_Body::SetMass(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float mass = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetMass(mass); });

    return 0;
}

int Wrap_Body::SetInertia(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float inertia = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() { self->SetInertia(inertia); });

    return 0;
}

int Wrap_Body::SetAngularDamping(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float damping = luaL_checknumber(L, 2);

    self->SetAngularDamping(damping);

    return 0;
}

int Wrap_Body::SetLinearDamping(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float damping = luaL_checknumber(L, 2);

    self->SetLinearDamping(damping);

    return 0;
}

int Wrap_Body::SetGravityScale(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float scale = luaL_checknumber(L, 2);

    self->SetGravityScale(scale);

    return 0;
}

int Wrap_Body::SetType(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    auto type = Body::bodyTypes.Find(luaL_checkstring(L, 2));

    luax::CatchException(L, [&]() { self->SetType(*type); });

    return 0;
}

int Wrap_Body::GetWorldPoint(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float worldX, worldY = 0.0f;
    self->GetWorldPoint(x, y, worldX, worldY);

    lua_pushnumber(L, worldX);
    lua_pushnumber(L, worldY);

    return 2;
}

int Wrap_Body::GetWorldVector(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float worldX, worldY = 0.0f;
    self->GetWorldVector(x, y, worldX, worldY);

    lua_pushnumber(L, worldX);
    lua_pushnumber(L, worldY);

    return 2;
}

int Wrap_Body::GetWorldPoints(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetWorldPoints(L);
}

int Wrap_Body::GetLocalPoint(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float localX, localY = 0.0f;
    self->GetLocalPoint(x, y, localX, localY);

    lua_pushnumber(L, localX);
    lua_pushnumber(L, localY);

    return 2;
}

int Wrap_Body::GetLocalVector(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float localX, localY = 0.0f;
    self->GetLocalVector(x, y, localX, localY);

    lua_pushnumber(L, localX);
    lua_pushnumber(L, localY);

    return 2;
}

int Wrap_Body::GetLocalPoints(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetLocalPoints(L);
}

int Wrap_Body::GetLinearVelocityFromWorldPoint(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float velocityX, velocityY = 0.0f;
    self->GetLinearVelocityFromWorldPoint(x, y, velocityX, velocityY);

    lua_pushnumber(L, velocityX);
    lua_pushnumber(L, velocityY);

    return 2;
}

int Wrap_Body::GetLinearVelocityFromLocalPoint(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    float velocityX, velocityY = 0.0f;
    self->GetLinearVelocityFromLocalPoint(x, y, velocityX, velocityY);

    lua_pushnumber(L, velocityX);
    lua_pushnumber(L, velocityY);

    return 2;
}

int Wrap_Body::IsBullet(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    lua_pushboolean(L, self->IsBullet());

    return 1;
}

int Wrap_Body::SetBullet(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    bool bullet = luax::CheckBoolean(L, 2);
    self->SetBullet(bullet);

    return 0;
}

int Wrap_Body::IsActive(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    luax::PushBoolean(L, self->IsEnabled());

    return 1;
}

int Wrap_Body::IsAwake(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    luax::PushBoolean(L, self->IsAwake());

    return 1;
}

int Wrap_Body::SetSleepingAllowed(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    bool allowed = luax::CheckBoolean(L, 2);
    self->SetSleepingAllowed(allowed);

    return 0;
}

int Wrap_Body::IsSleepingAllowed(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    luax::PushBoolean(L, self->IsSleepingAllowed());

    return 1;
}

int Wrap_Body::SetActive(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    bool active = luax::CheckBoolean(L, 2);
    self->SetEnabled(active);

    return 0;
}

int Wrap_Body::SetAwake(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    bool awake = luax::CheckBoolean(L, 2);
    self->SetAwake(awake);

    return 0;
}

int Wrap_Body::SetFixedRotation(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    bool fixed = luax::CheckBoolean(L, 2);
    luax::CatchException(L, [&]() { self->SetFixedRotation(fixed); });

    return 0;
}

int Wrap_Body::IsFixedRotation(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    luax::PushBoolean(L, self->IsFixedRotation());

    return 1;
}

int Wrap_Body::IsTouching(lua_State* L)
{
    auto* self  = Wrap_Body::CheckBody(L, 1);
    auto* other = Wrap_Body::CheckBody(L, 2);

    luax::PushBoolean(L, self->IsTouching(other));

    return 1;
}

int Wrap_Body::GetWorld(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    auto* world = self->GetWorld();
    luax::PushType(L, world);

    return 1;
}

int Wrap_Body::GetShape(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    auto* shape = self->GetShape();

    if (shape)
        Wrap_Shape::PushShape(L, shape);
    else
        lua_pushnil(L);

    return 1;
}

int Wrap_Body::GetShapes(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetShapes(L); });

    return count;
}

int Wrap_Body::GetJoints(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetJoints(L); });

    return count;
}

int Wrap_Body::GetContacts(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    int count = 0;
    luax::CatchException(L, [&]() { count = self->GetContacts(L); });

    return count;
}

int Wrap_Body::Destroy(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    luax::CatchException(L, [&]() { self->Destroy(); });

    return 0;
}

int Wrap_Body::IsDestroyed(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);

    luax::PushBoolean(L, self->body == nullptr);

    return 1;
}

int Wrap_Body::SetUserdata(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->SetUserdata(L);
}

int Wrap_Body::GetUserdata(lua_State* L)
{
    auto* self = Wrap_Body::CheckBody(L, 1);
    lua_remove(L, 1);

    return self->GetUserdata(L);
}

// clang-format off
const luaL_Reg Wrap_Body::bodyFunctions[0x42] =
{
    { "getX",                            Wrap_Body::GetX                            },
    { "getY",                            Wrap_Body::GetY                            },
    { "getAngle",                        Wrap_Body::GetAngle                        },
    { "getPosition",                     Wrap_Body::GetPosition                     },
    { "getTransform",                    Wrap_Body::GetTransform                    },
    { "getLinearVelocity",               Wrap_Body::GetLinearVelocity               },
    { "getWorldCenter",                  Wrap_Body::GetWorldCenter                  },
    { "getLocalCenter",                  Wrap_Body::GetLocalCenter                  },
    { "getAngularVelocity",              Wrap_Body::GetAngularVelocity              },
    { "getKinematicState",               Wrap_Body::GetKinematicState               },
    { "getMass",                         Wrap_Body::GetMass                         },
    { "getInertia",                      Wrap_Body::GetInertia                      },
    { "getMassData",                     Wrap_Body::GetMassData                     },
    { "hasCustomMassData",               Wrap_Body::HasCustomMassData               },
    { "getAngularDamping",               Wrap_Body::GetAngularDamping               },
    { "getLinearDamping",                Wrap_Body::GetLinearDamping                },
    { "getGravityScale",                 Wrap_Body::GetGravityScale                 },
    { "getType",                         Wrap_Body::GetType                         },
    { "applyLinearImpulse",              Wrap_Body::ApplyLinearImpulse              },
    { "applyAngularImpulse",             Wrap_Body::ApplyAngularImpulse             },
    { "applyTorque",                     Wrap_Body::ApplyTorque                     },
    { "applyForce",                      Wrap_Body::ApplyForce                      },
    { "setX",                            Wrap_Body::SetX                            },
    { "setY",                            Wrap_Body::SetY                            },
    { "setTransform",                    Wrap_Body::SetTransform                    },
    { "setLinearVelocity",               Wrap_Body::SetLinearVelocity               },
    { "setAngle",                        Wrap_Body::SetAngle                        },
    { "setAngularVelocity",              Wrap_Body::SetAngularVelocity              },
    { "setPosition",                     Wrap_Body::SetPosition                     },
    { "setKinematicState",               Wrap_Body::SetKinematicState               },
    { "resetMassData",                   Wrap_Body::ResetMassData                   },
    { "setMassData",                     Wrap_Body::SetMassData                     },
    { "setMass",                         Wrap_Body::SetMass                         },
    { "setInertia",                      Wrap_Body::SetInertia                      },
    { "setAngularDamping",               Wrap_Body::SetAngularDamping               },
    { "setLinearDamping",                Wrap_Body::SetLinearDamping                },
    { "setGravityScale",                 Wrap_Body::SetGravityScale                 },
    { "setType",                         Wrap_Body::SetType                         },
    { "getWorldPoint",                   Wrap_Body::GetWorldPoint                   },
    { "getWorldVector",                  Wrap_Body::GetWorldVector                  },
    { "getWorldPoints",                  Wrap_Body::GetWorldPoints                  },
    { "getLocalPoint",                   Wrap_Body::GetLocalPoint                   },
    { "getLocalVector",                  Wrap_Body::GetLocalVector                  },
    { "getLocalPoints",                  Wrap_Body::GetLocalPoints                  },
    { "getLinearVelocityFromWorldPoint", Wrap_Body::GetLinearVelocityFromWorldPoint },
    { "getLinearVelocityFromLocalPoint", Wrap_Body::GetLinearVelocityFromLocalPoint },
    { "isBullet",                        Wrap_Body::IsBullet                        },
    { "setBullet",                       Wrap_Body::SetBullet                       },
    { "isActive",                        Wrap_Body::IsActive                        },
    { "isAwake",                         Wrap_Body::IsAwake                         },
    { "setSleepingAllowed",              Wrap_Body::SetSleepingAllowed              },
    { "isSleepingAllowed",               Wrap_Body::IsSleepingAllowed               },
    { "setActive",                       Wrap_Body::SetActive                       },
    { "setAwake",                        Wrap_Body::SetAwake                        },
    { "setFixedRotation",                Wrap_Body::SetFixedRotation                },
    { "isFixedRotation",                 Wrap_Body::IsFixedRotation                 },
    { "isTouching",                      Wrap_Body::IsTouching                      },
    { "getWorld",                        Wrap_Body::GetWorld                        },
    { "getShape",                        Wrap_Body::GetShape                        },
    { "getShapes",                       Wrap_Body::GetShapes                       },
    { "getJoints",                       Wrap_Body::GetJoints                       },
    { "getContacts",                     Wrap_Body::GetContacts                     },
    { "destroy",                         Wrap_Body::Destroy                         },
    { "isDestroyed",                     Wrap_Body::IsDestroyed                     },
    { "setUserData",                     Wrap_Body::SetUserdata                     },
    { "getUserData",                     Wrap_Body::GetUserdata                     }
};
//clang-format on

int Wrap_Body::Register(lua_State*L)
{
    return luax::RegisterType(L, &Body::type, Wrap_Body::bodyFunctions);
}
