#include "modules/math/wrap_MathModule.hpp"

#include "modules/math/wrap_BezierCurve.hpp"
#include "modules/math/wrap_RandomGenerator.hpp"
#include "modules/math/wrap_Transform.hpp"

#include <algorithm>
#include <cmath>

static constexpr char wrap_math_lua[] = {
#include "modules/math/wrap_Math.lua"
};

using namespace love;

#define instance() (Module::getInstance<Math>(Module::M_MATH))

int Wrap_MathModule::_getRandomGenerator(lua_State* L)
{
    auto* random = instance()->getRandomGenerator();
    luax_pushtype(L, random);

    return 1;
}

int Wrap_MathModule::newRandomGenerator(lua_State* L)
{
    RandomGenerator::Seed seed {};
    if (lua_gettop(L) > 0)
        seed = luax_checkrandomseed(L, 1);

    RandomGenerator* random = instance()->newRandomGenerator();

    if (lua_gettop(L) > 0)
    {
        bool shouldError = false;

        try
        {
            random->setSeed(seed);
        }
        catch (love::Exception& e)
        {
            random->release();
            shouldError = true;
            lua_pushstring(L, e.what());
        }

        if (shouldError)
            return luaL_error(L, "%s", lua_tostring(L, -1));
    }

    luax_pushtype(L, random);
    random->release();

    return 1;
}

int Wrap_MathModule::newBezierCurve(lua_State* L)
{
    std::vector<Vector2> points {};

    if (lua_istable(L, 1))
    {
        int top = luax_objlen(L, 1);
        points.reserve(top / 2);

        for (int index = 1; index <= top; index += 2)
        {
            lua_rawgeti(L, 1, index);
            lua_rawgeti(L, 1, index + 1);

            Vector2 point {};
            point.x = luaL_checknumber(L, -2);
            point.y = luaL_checknumber(L, -1);

            points.push_back(point);

            lua_pop(L, 2);
        }
    }
    else
    {
        int top = lua_gettop(L);
        points.reserve(top / 2);

        for (int index = 1; index <= top; index += 2)
        {
            Vector2 point {};
            point.x = luaL_checknumber(L, index);
            point.y = luaL_checknumber(L, index + 1);

            points.push_back(point);
        }
    }

    auto* bezierCurve = instance()->newBezierCurve(points);

    luax_pushtype(L, bezierCurve);
    bezierCurve->release();

    return 1;
}

int Wrap_MathModule::newTransform(lua_State* L)
{
    Transform* transform = nullptr;

    if (lua_isnoneornil(L, 1))
        transform = instance()->newTransform();
    else
    {
        float x  = (float)luaL_checknumber(L, 1);
        float y  = (float)luaL_checknumber(L, 2);
        float a  = (float)luaL_optnumber(L, 3, 0.0);
        float sx = (float)luaL_optnumber(L, 4, 1.0);
        float sy = (float)luaL_optnumber(L, 5, sx);
        float ox = (float)luaL_optnumber(L, 6, 0.0);
        float oy = (float)luaL_optnumber(L, 7, 0.0);
        float kx = (float)luaL_optnumber(L, 8, 0.0);
        float ky = (float)luaL_optnumber(L, 9, 0.0);

        transform = instance()->newTransform(x, y, a, sx, sy, ox, oy, kx, ky);
    }

    luax_pushtype(L, transform);
    transform->release();

    return 1;
}

int Wrap_MathModule::triangulate(lua_State* L)
{
    std::vector<Vector2> vertices {};

    if (lua_istable(L, 1))
    {
        int top = luax_objlen(L, 1);
        vertices.reserve(top / 2);

        for (int index = 1; index <= top; index += 2)
        {
            lua_rawgeti(L, 1, index);
            lua_rawgeti(L, 1, index + 1);

            Vector2 point {};
            point.x = luaL_checknumber(L, -2);
            point.y = luaL_checknumber(L, -1);

            vertices.push_back(point);

            lua_pop(L, 2);
        }
    }
    else
    {
        int top = lua_gettop(L);
        vertices.reserve(top / 2);

        for (int index = 1; index <= top; index += 2)
        {
            Vector2 point {};
            point.x = luaL_checknumber(L, index);
            point.y = luaL_checknumber(L, index + 1);

            vertices.push_back(point);
        }
    }

    if (vertices.size() < 3)
        return luaL_error(L, "Need at least 3 vertices to triangulate");

    std::vector<Triangle> triangles {};

    luax_catchexcept(L, [&]() {
        if (vertices.size() == 3)
            triangles.push_back(Triangle(vertices[0], vertices[1], vertices[2]));
        else
            triangles = love::triangulate(vertices);
    });

    lua_createtable(L, triangles.size(), 0);

    for (int index = 0; index < (int)triangles.size(); ++index)
    {
        const Triangle& triangle = triangles[index];

        lua_createtable(L, 6, 0);

        lua_pushnumber(L, triangle.a.x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, triangle.a.y);
        lua_rawseti(L, -2, 2);

        lua_pushnumber(L, triangle.b.x);
        lua_rawseti(L, -2, 3);

        lua_pushnumber(L, triangle.b.y);
        lua_rawseti(L, -2, 4);

        lua_pushnumber(L, triangle.c.x);
        lua_rawseti(L, -2, 5);

        lua_pushnumber(L, triangle.c.y);
        lua_rawseti(L, -2, 6);

        lua_rawseti(L, -2, index + 1);
    }

    return 1;
}

int Wrap_MathModule::isConvex(lua_State* L)
{
    std::vector<Vector2> vertices {};

    if (lua_istable(L, 1))
    {
        int top = luax_objlen(L, 1);
        vertices.reserve(top / 2);

        for (int index = 1; index <= top; index += 2)
        {
            lua_rawgeti(L, 1, index);
            lua_rawgeti(L, 1, index + 1);

            Vector2 point {};
            point.x = luaL_checknumber(L, -2);
            point.y = luaL_checknumber(L, -1);

            vertices.push_back(point);

            lua_pop(L, 2);
        }
    }
    else
    {
        int top = lua_gettop(L);
        vertices.reserve(top / 2);

        for (int index = 1; index <= top; index += 2)
        {
            Vector2 point {};
            point.x = luaL_checknumber(L, index);
            point.y = luaL_checknumber(L, index + 1);

            vertices.push_back(point);
        }
    }

    luax_pushboolean(L, love::isConvex(vertices));

    return 1;
}

static int getGammaArguments(lua_State* L, float color[4])
{
    int numComponents = 0;

    if (lua_istable(L, 1))
    {
        int length = luax_objlen(L, 1);

        for (int index = 1; index <= length; index++)
        {
            lua_rawgeti(L, 1, index);
            color[index - 1] = luax_checknumberclamped01(L, -1);
            numComponents++;
        }

        lua_pop(L, numComponents);
    }
    else
    {
        int length = lua_gettop(L);

        for (int index = 1; index <= length && index <= 4; index++)
        {
            color[index - 1] = luax_checknumberclamped01(L, index);
            numComponents++;
        }
    }

    if (numComponents == 0)
        luaL_checknumber(L, 1);

    return numComponents;
}

int Wrap_MathModule::gammaToLinear(lua_State* L)
{
    float color[4] {};
    int numComponents = getGammaArguments(L, color);

    for (int index = 0; index < numComponents; index++)
    {
        if (index < 3)
            color[index] = love::gammaToLinear(color[index]);

        lua_pushnumber(L, color[index]);
    }

    return numComponents;
}

int Wrap_MathModule::linearToGamma(lua_State* L)
{
    float color[4] {};
    int numComponents = getGammaArguments(L, color);

    for (int index = 0; index < numComponents; index++)
    {
        if (index < 3)
            color[index] = love::linearToGamma(color[index]);

        lua_pushnumber(L, color[index]);
    }

    return numComponents;
}

int Wrap_MathModule::simplexNoise(lua_State* L)
{
    int argc = std::clamp(lua_gettop(L), 1, 4);
    double args[4] {};

    for (int index = 0; index < argc; index++)
        args[index] = luaL_checknumber(L, index + 1);

    double value = 0.0;

    switch (argc)
    {
        case 1:
            value = love::simplexNoise1(args[0]);
            break;
        case 2:
            value = love::simplexNoise2(args[0], args[1]);
            break;
        case 3:
            value = love::simplexNoise3(args[0], args[1], args[2]);
            break;
        case 4:
            value = love::simplexNoise4(args[0], args[1], args[2], args[3]);
            break;
    }

    lua_pushnumber(L, value);

    return 1;
}

int Wrap_MathModule::perlinNoise(lua_State* L)
{
    int argc = std::clamp(lua_gettop(L), 1, 4);
    double args[4] {};

    for (int index = 0; index < argc; index++)
        args[index] = luaL_checknumber(L, index + 1);

    double value = 0.0;

    switch (argc)
    {
        case 1:
            value = love::perlinNoise1(args[0]);
            break;
        case 2:
            value = love::perlinNoise2(args[0], args[1]);
            break;
        case 3:
            value = love::perlinNoise3(args[0], args[1], args[2]);
            break;
        case 4:
            value = love::perlinNoise4(args[0], args[1], args[2], args[3]);
            break;
    }

    lua_pushnumber(L, value);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "_getRandomGenerator", Wrap_MathModule::_getRandomGenerator },
    { "newRandomGenerator",  Wrap_MathModule::newRandomGenerator  },
    { "newBezierCurve",      Wrap_MathModule::newBezierCurve      },
    { "newTransform",        Wrap_MathModule::newTransform        },
    { "triangulate",         Wrap_MathModule::triangulate         },
    { "isConvex",            Wrap_MathModule::isConvex            },
    { "gammaToLinear",       Wrap_MathModule::gammaToLinear       },
    { "linearToGamma",       Wrap_MathModule::linearToGamma       },
    { "simplexNoise",        Wrap_MathModule::simplexNoise        },
    { "perlinNoise",         Wrap_MathModule::perlinNoise         }
};

static constexpr lua_CFunction types[] =
{
    love::open_randomgenerator,
    love::open_beziercurve,
    love::open_transform
};
// clang-format on

int Wrap_MathModule::open(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax_catchexcept(L, [&]() { instance = new Math(); });
    else
        instance->retain();

    WrappedModule module {};
    module.instance  = instance;
    module.name      = "math";
    module.type      = &Module::type;
    module.functions = functions;
    module.types     = types;

    int result = luax_register_module(L, module);

    luaL_loadbuffer(L, wrap_math_lua, sizeof(wrap_math_lua), "=[love \"wrap_Math.lua\"]");
    lua_pushvalue(L, -2);
    lua_pushnil(L);
    lua_call(L, 2, 0);

    return result;
}
