#include "objects/box2d/contact/wrap_contact.h"

using namespace love;

Contact* Wrap_Contact::CheckContact(lua_State* L, int index)
{
    Contact* contact = Luax::CheckType<Contact>(L, index);
    if (!contact->IsValid())
        luaL_error(L, "Attempt to use destroyed contact.");

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

    lua_pushboolean(L, self->IsEnabled());

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

    lua_pushboolean(L, !contact->IsValid());

    return 1;
}

int Wrap_Contact::Register(lua_State* L)
{
    luaL_Reg funcs[] = { { "getPositions", GetPositions },
                         { "getNormal", GetNormal },
                         { "getFriction", GetFriction },
                         { "getRestitution", GetRestitution },
                         { "isEnabled", IsEnabled },
                         { "isTouching", IsTouching },
                         { "setFriction", SetFriction },
                         { "setRestitution", SetRestitution },
                         { "setEnabled", SetEnabled },
                         { "resetFriction", ResetFriction },
                         { "resetRestitution", ResetRestitution },
                         { "setTangentSpeed", SetTangentSpeed },
                         { "getTangentSpeed", GetTangentSpeed },
                         { "getChildren", GetChildren },
                         { "getFixtures", GetFixtures },
                         { "isDestroyed", IsDestroyed },
                         { 0, 0 } };

    return Luax::RegisterType(L, &Contact::type, funcs, nullptr);
}
