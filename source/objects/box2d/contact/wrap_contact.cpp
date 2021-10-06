#include "objects/box2d/contact/wrap_contact.h"

using namespace love;

Contact* Wrap_Contact::CheckContact(lua_State* L, int index)
{
    Contact* contact = Luax::CheckType<Contact>(L, index);
    if (!contact->IsValid())
        luaL_error(L, "Attempt to use destroyed contact!");

    return contact;
}

int Wrap_Contact::GetPositions(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    return self->GetPositions(L);
}

int Wrap_Contact::GetNormal(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    return self->GetNormal(L);
}

int Wrap_Contact::GetFriction(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    lua_pushnumber(L, self->GetFriction());

    return 1;
}

int Wrap_Contact::GetRestitution(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    lua_pushnumber(L, self->GetRestitution());

    return 1;
}

int Wrap_Contact::IsEnabled(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    Luax::PushBoolean(L, self->IsEnabled());

    return 1;
}

int Wrap_Contact::ResetFriction(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);
    self->ResetFriction();

    return 0;
}

int Wrap_Contact::SetTangentSpeed(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);
    float speed   = luaL_checknumber(L, 2);

    self->SetTangentSpeed(speed);

    return 0;
}

int Wrap_Contact::GetTangentSpeed(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    lua_pushnumber(L, self->GetTangentSpeed());

    return 1;
}

int Wrap_Contact::GetChildren(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);
    int a, b;

    self->GetChildren(a, b);

    lua_pushnumber(L, a + 1);
    lua_pushnumber(L, b + 1);

    return 2;
}

int Wrap_Contact::GetFixtures(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    Fixture* a = nullptr;
    Fixture* b = nullptr;

    Luax::CatchException(L, [&]() { self->GetFixtures(a, b); });

    Luax::PushType(L, a);
    Luax::PushType(L, b);

    return 2;
}

int Wrap_Contact::IsDestroyed(lua_State* L)
{
    Contact* contact = Luax::CheckType<Contact>(L, 1);

    Luax::PushBoolean(L, !contact->IsValid());

    return 1;
}

int Wrap_Contact::IsTouching(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);
    Luax::PushBoolean(L, self->IsTouching());

    return 1;
}

int Wrap_Contact::SetFriction(lua_State* L)
{
    Contact* self  = Wrap_Contact::CheckContact(L, 1);
    float friction = (float)luaL_checknumber(L, 2);

    self->SetFriction(friction);

    return 0;
}

int Wrap_Contact::SetRestitution(lua_State* L)
{
    Contact* self     = Wrap_Contact::CheckContact(L, 1);
    float restitution = (float)luaL_checknumber(L, 2);

    self->SetRestitution(restitution);

    return 0;
}

int Wrap_Contact::SetEnabled(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);
    bool enabled  = Luax::CheckBoolean(L, 2);

    self->SetEnabled(enabled);

    return 0;
}

int Wrap_Contact::ResetRestitution(lua_State* L)
{
    Contact* self = Wrap_Contact::CheckContact(L, 1);

    self->ResetRestitution();

    return 0;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getChildren",      Wrap_Contact::GetChildren      },
    { "getFixtures",      Wrap_Contact::GetFixtures      },
    { "getFriction",      Wrap_Contact::GetFriction      },
    { "getNormal",        Wrap_Contact::GetNormal        },
    { "getPositions",     Wrap_Contact::GetPositions     },
    { "getRestitution",   Wrap_Contact::GetRestitution   },
    { "getTangentSpeed",  Wrap_Contact::GetTangentSpeed  },
    { "isDestroyed",      Wrap_Contact::IsDestroyed      },
    { "isEnabled",        Wrap_Contact::IsEnabled        },
    { "isTouching",       Wrap_Contact::IsTouching       },
    { "resetFriction",    Wrap_Contact::ResetFriction    },
    { "resetRestitution", Wrap_Contact::ResetRestitution },
    { "setEnabled",       Wrap_Contact::SetEnabled       },
    { "setFriction",      Wrap_Contact::SetFriction      },
    { "setRestitution",   Wrap_Contact::SetRestitution   },
    { "setTangentSpeed",  Wrap_Contact::SetTangentSpeed  },
    { 0,                  0                              }
};
// clang-format on

int Wrap_Contact::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Contact::type, functions, nullptr);
}
