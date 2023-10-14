#include <modules/physics/wrap_physics.hpp>

#include <objects/body/wrap_body.hpp>
#include <objects/contact/wrap_contact.hpp>
#include <objects/joint/wrap_joint.hpp>
#include <objects/shape/wrap_shape.hpp>
#include <objects/world/wrap_world.hpp>

using namespace love;

#define instance() (Module::GetInstance<Physics>(Module::M_PHYSICS))

int Wrap_Physics::NewWorld(lua_State* L)
{
    float gravityX = luaL_optnumber(L, 1, 0.0f);
    float gravityY = luaL_optnumber(L, 2, 0.0f);
    bool sleep     = luax::OptBoolean(L, 3, true);

    World* world = nullptr;

    luax::CatchException(L, [&]() { world = instance()->NewWorld(gravityX, gravityY, sleep); });

    luax::PushType(L, world);
    world->Release();

    return 1;
}

// #region Body

int Wrap_Physics::NewBody(lua_State* L)
{
    auto* world = Wrap_World::CheckWorld(L, 1);
    float x     = luaL_optnumber(L, 2, 0.0f);
    float y     = luaL_optnumber(L, 3, 0.0f);

    const char* typeString = lua_isnoneornil(L, 4) ? nullptr : luaL_checkstring(L, 4);

    std::optional<Body::Type> bodyType(Body::BODY_STATIC);
    if (typeString && (!(bodyType = Body::bodyTypes.Find(typeString))))
        return luax::EnumError(L, "Body type", Body::bodyTypes, typeString);

    Body* body = nullptr;
    luax::CatchException(L, [&]() { body = instance()->NewBody(world, x, y, *bodyType); });

    luax::PushType(L, body);
    body->Release();

    return 1;
}

// #endregion Body

// #region Shape

static Body* optBodyForShape(lua_State* L, int index)
{
    if (lua_isnoneornil(L, index) || luax::IsType(L, index, Object::type))
        return Wrap_Body::CheckBody(L, index);

    return nullptr;
}

// #endregion Shape

// #region Joint

// #endregion Joint

int Wrap_Physics::GetDistance(lua_State* L)
{
    return instance()->GetDistance(L);
}

int Wrap_Physics::SetMeter(lua_State* L)
{
    float meter = luaL_checknumber(L, 1);
    instance()->SetMeter(meter);

    return 0;
}

int Wrap_Physics::GetMeter(lua_State* L)
{
    lua_pushnumber(L, instance()->GetMeter());

    return 1;
}

int Wrap_Physics::ComputeLinearStiffness(lua_State* L)
{
    float frequency    = luaL_checknumber(L, 1);
    float dampingRatio = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float siffness, damping = 0.0f;
    Physics::ComputeLinearStiffness(siffness, damping, frequency, dampingRatio, first->body,
                                    second);

    lua_pushnumber(L, siffness);
    lua_pushnumber(L, damping);

    return 2;
}

int Wrap_Physics::ComputeLinearFrequency(lua_State* L)
{
    float stiffness = luaL_checknumber(L, 1);
    float damping   = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float frequency, dampingRatio = 0.0f;
    Physics::ComputeLinearFrequency(frequency, dampingRatio, stiffness, damping, first->body,
                                    second);

    lua_pushnumber(L, frequency);
    lua_pushnumber(L, dampingRatio);

    return 2;
}

int Wrap_Physics::ComputeAngularStiffness(lua_State* L)
{
    float frequency    = luaL_checknumber(L, 1);
    float dampingRatio = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float siffness, damping = 0.0f;
    Physics::ComputeAngularStiffness(siffness, damping, frequency, dampingRatio, first->body,
                                     second);

    lua_pushnumber(L, siffness);
    lua_pushnumber(L, damping);

    return 2;
}

int Wrap_Physics::ComputeAngularFrequency(lua_State* L)
{
    float stiffness = luaL_checknumber(L, 1);
    float damping   = luaL_checknumber(L, 2);

    Body* first    = Wrap_Body::CheckBody(L, 3);
    b2Body* second = nullptr;

    if (lua_isnoneornil(L, 4))
        second = first->GetWorld()->GetGroundBody();
    else
        second = Wrap_Body::CheckBody(L, 4)->body;

    float frequency, dampingRatio = 0.0f;
    Physics::ComputeAngularFrequency(frequency, dampingRatio, stiffness, damping, first->body,
                                     second);

    lua_pushnumber(L, frequency);
    lua_pushnumber(L, dampingRatio);

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "newWorld",                Wrap_Physics::NewWorld                },
    { "newBody",                 Wrap_Physics::NewBody                 },
    { "getDistance",             Wrap_Physics::GetDistance             },
    { "getMeter",                Wrap_Physics::GetMeter                },
    { "setMeter",                Wrap_Physics::SetMeter                },
    { "computeLinearStiffness",  Wrap_Physics::ComputeLinearStiffness  },
    { "computeLinearFrequency",  Wrap_Physics::ComputeLinearFrequency  },
    { "computeAngularStiffness", Wrap_Physics::ComputeAngularStiffness },
    { "computeAngularFrequency", Wrap_Physics::ComputeAngularFrequency }
};

static constexpr lua_CFunction types[] =
{
    Wrap_World::Register,
    Wrap_Contact::Register,
    Wrap_Body::Register,
    Wrap_Shape::Register,
    Wrap_Joint::Register,
    nullptr
};
// clang-format on

int Wrap_Physics::Register(lua_State* L)
{
    Physics* instance = instance();
    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Physics(); });
    else
        instance->Retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "physics";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    return luax::RegisterModule(L, module);
}
