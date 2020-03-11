#include "common/runtime.h"
#include "modules/math/wrap_mathmodule.h"

#include "wrap_math_lua.h"

using namespace love;

#define instance() (Module::GetInstance<Math>(Module::M_MATH))

int Wrap_Math::GetRandomGenerator(lua_State * L)
{
    RandomGenerator * randomGenerator = instance()->GetRandomGenerator();

    Luax::PushType(L, randomGenerator);

    return 1;
}

int Wrap_Math::IsConvex(lua_State * L)
{
    std::vector<love::Vector2> vertices;

    if (lua_istable(L, 1))
    {
        int top = (int)lua_objlen(L, 1);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            lua_rawgeti(L, 1, i);
            lua_rawgeti(L, 1, i+1);

            love::Vector2 v;
            v.x = (float)luaL_checknumber(L, -2);
            v.y = (float)luaL_checknumber(L, -1);

            vertices.push_back(v);

            lua_pop(L, 2);
        }
    }
    else
    {
        int top = lua_gettop(L);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            love::Vector2 v;
            v.x = (float)luaL_checknumber(L, i);
            v.y = (float)luaL_checknumber(L, i+1);

            vertices.push_back(v);
        }
    }

    lua_pushboolean(L, instance()->IsConvex(vertices));

    return 1;
}

int Wrap_Math::NewRandomGenerator(lua_State * L)
{
    RandomGenerator::Seed seed;

    if (lua_gettop(L) > 0)
        seed = Wrap_RandomGenerator::CheckRandomSeed(L, 1);

    RandomGenerator * rng = instance()->NewRandomGenerator();

    if (lua_gettop(L) > 0)
    {
        bool shouldError = false;

        try
        {
            rng->SetSeed(seed);
        }
        catch (love::Exception & e)
        {
            rng->Release();
            shouldError = true;
            lua_pushstring(L, e.what());
        }

        if (shouldError)
            return luaL_error(L, "%s", lua_tostring(L, -1));
    }

    Luax::PushType(L, rng);
    rng->Release();

    return 1;
}

int Wrap_Math::Noise(lua_State * L)
{
    int argc = std::min(std::max(lua_gettop(L), 1), 4);
    float args[4];

    for (int i = 0; i < argc; i++)
        args[i] = (float)luaL_checknumber(L, i + 1);

    float value = 0.0f;

    switch (argc)
    {
        case 1:
            value = SimplexNoise1234::noise(args[0]) * 0.5f + 0.5f;
            break;
        case 2:
            value = SimplexNoise1234::noise(args[0], args[1]) * 0.5f + 0.5f;
            break;
        case 3:
            value = Noise1234::noise(args[0], args[1], args[2]) * 0.5f + 0.5f;
            break;
        case 4:
            value = Noise1234::noise(args[0], args[1], args[2], args[3]) * 0.5f + 0.5f;
            break;
    }

    lua_pushnumber(L, (lua_Number)value);

    return 1;
}

int Wrap_Math::Triangulate(lua_State * L)
{
    std::vector<love::Vector2> vertices;

    if (lua_istable(L, 1))
    {
        int top = (int)lua_objlen(L, 1);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            lua_rawgeti(L, 1, i);
            lua_rawgeti(L, 1, i + 1);

            Vector2 v;
            v.x = (float) luaL_checknumber(L, -2);
            v.y = (float) luaL_checknumber(L, -1);

            vertices.push_back(v);

            lua_pop(L, 2);
        }
    }
    else
    {
        int top = (int) lua_gettop(L);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            Vector2 v;
            v.x = (float) luaL_checknumber(L, i);
            v.y = (float) luaL_checknumber(L, i + 1);

            vertices.push_back(v);
        }
    }

    if (vertices.size() < 3)
        return luaL_error(L, "Need at least 3 vertices to triangulate");

    std::vector<Math::Triangle> triangles;

    Luax::CatchException(L, [&]() {
        if (vertices.size() == 3)
            triangles.push_back(Math::Triangle(vertices[0], vertices[1], vertices[2]));
        else
            triangles = instance()->Triangulate(vertices);
    });

    lua_createtable(L, (int)triangles.size(), 0);

    for (int i = 0; i < (int)triangles.size(); ++i)
    {
        const Math::Triangle &tri = triangles[i];

        lua_createtable(L, 6, 0);
        lua_pushnumber(L, tri.a.x);

        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, tri.a.y);

        lua_rawseti(L, -2, 2);
        lua_pushnumber(L, tri.b.x);

        lua_rawseti(L, -2, 3);
        lua_pushnumber(L, tri.b.y);

        lua_rawseti(L, -2, 4);
        lua_pushnumber(L, tri.c.x);

        lua_rawseti(L, -2, 5);
        lua_pushnumber(L, tri.c.y);

        lua_rawseti(L, -2, 6);
        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int Wrap_Math::Register(lua_State * L)
{
    luaL_Reg reg[] =
    {
        { "_getRandomGenerator", GetRandomGenerator },
        { "isConvex",            IsConvex           },
        { "newRandomGenerator",  NewRandomGenerator },
        { "triangulate",         Triangulate        },
        { 0,                     0                  }
    };

    lua_CFunction types[] =
    {
        Wrap_RandomGenerator::Register,
        0
    };

    Math * instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Math(); });
    else
        instance->Retain();

    WrappedModule module;

    module.instance = instance;
    module.name = "math";
    module.functions = reg;
    module.type = &Module::type;
    module.types = types;

    int ret = Luax::RegisterModule(L, module);

    luaL_loadbuffer(L, (const char *)wrap_math_lua, wrap_math_lua_size, "wrap_math.lua");
    lua_pushvalue(L, -2);
    lua_call(L, 1, 0);

    return ret;
}
