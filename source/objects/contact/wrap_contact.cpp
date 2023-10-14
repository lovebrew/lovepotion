#include <objects/contact/wrap_contact.hpp>

#include <objects/shape/wrap_shape.hpp>

using namespace love;

Contact* Wrap_Contact::CheckContact(lua_State* L, int index)
{
    auto* contact = luax::CheckType<Contact>(L, index);

    if (!contact->IsValid())
        luaL_error(L, "Attempt to use destroyed contact.");

    return contact;
}

int Wrap_Contact::GetPositions(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    return self->GetPositions(L);
}

int Wrap_Contact::GetNormal(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    return self->GetNormal(L);
}

int Wrap_Contact::GetFriction(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    lua_pushnumber(L, self->GetFriction());

    return 1;
}

int Wrap_Contact::GetRestitution(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    lua_pushnumber(L, self->GetRestitution());

    return 1;
}

int Wrap_Contact::IsEnabled(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    luax::PushBoolean(L, self->IsEnabled());

    return 1;
}

int Wrap_Contact::IsTouching(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    luax::PushBoolean(L, self->IsTouching());

    return 1;
}

int Wrap_Contact::SetFriction(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    float friction = luaL_checknumber(L, 2);
    self->SetFriction(friction);

    return 0;
}

int Wrap_Contact::SetRestitution(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    float restitution = luaL_checknumber(L, 2);
    self->SetRestitution(restitution);

    return 0;
}

int Wrap_Contact::SetEnabled(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    bool enabled = luax::CheckBoolean(L, 2);
    self->SetEnabled(enabled);

    return 0;
}

int Wrap_Contact::ResetFriction(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    self->ResetFriction();

    return 0;
}

int Wrap_Contact::ResetRestitution(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    self->ResetRestitution();

    return 0;
}

int Wrap_Contact::SetTangentSpeed(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    float speed = luaL_checknumber(L, 2);
    self->SetTangentSpeed(speed);

    return 0;
}

int Wrap_Contact::GetTangentSpeed(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    lua_pushnumber(L, self->GetTangentSpeed());

    return 1;
}

int Wrap_Contact::GetChildren(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    int a, b = 0;
    self->GetChildren(a, b);

    lua_pushinteger(L, a + 1);
    lua_pushinteger(L, b + 1);

    return 2;
}

int Wrap_Contact::GetShapes(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    Shape* a = nullptr;
    Shape* b = nullptr;

    luax::CatchException(L, [&]() { self->GetShapes(a, b); });

    Wrap_Shape::PushShape(L, a);
    Wrap_Shape::PushShape(L, b);

    return 2;
}

int Wrap_Contact::IsDestroyed(lua_State* L)
{
    auto* self = Wrap_Contact::CheckContact(L, 1);

    luax::PushBoolean(L, !self->IsValid());

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getPositions",     Wrap_Contact::GetPositions     },
    { "getNormal",        Wrap_Contact::GetNormal        },
    { "getFriction",      Wrap_Contact::GetFriction      },
    { "getRestitution",   Wrap_Contact::GetRestitution   },
    { "isEnabled",        Wrap_Contact::IsEnabled        },
    { "isTouching",       Wrap_Contact::IsTouching       },
    { "setFriction",      Wrap_Contact::SetFriction      },
    { "setRestitution",   Wrap_Contact::SetRestitution   },
    { "setEnabled",       Wrap_Contact::SetEnabled       },
    { "resetFriction",    Wrap_Contact::ResetFriction    },
    { "resetRestitution", Wrap_Contact::ResetRestitution },
    { "setTangentSpeed",  Wrap_Contact::SetTangentSpeed  },
    { "getTangentSpeed",  Wrap_Contact::GetTangentSpeed  },
    { "getChildren",      Wrap_Contact::GetChildren      },
    { "getShapes",        Wrap_Contact::GetShapes        },
    { "isDestroyed",      Wrap_Contact::IsDestroyed      }
};
// clang-format on

int Wrap_Contact::Register(lua_State* L)
{
    return luax::RegisterType(L, &Contact::type, functions);
}
