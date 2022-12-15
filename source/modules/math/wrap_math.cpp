#include <modules/math/math.hpp>
#include <modules/math/wrap_math.hpp>

#include <objects/beziercurve/wrap_beziercurve.hpp>
#include <objects/randomgenerator/wrap_randomgenerator.hpp>
#include <objects/transform/wrap_transform.hpp>

using namespace love;

static constexpr char wrap_math_lua[] = {
#include "scripts/wrap_math.lua"
};

#define instance() (Module::GetInstance<Math>(Module::M_MATH))

int Wrap_Math::GetRandomGenerator(lua_State* L)
{
    auto* randomGenerator = instance()->GetRandomGenerator();

    luax::PushType(L, randomGenerator);

    return 1;
}

int Wrap_Math::NewRandomGenerator(lua_State* L)
{
    RandomGenerator::Seed seed {};

    if (lua_gettop(L) > 0)
        seed = Wrap_RandomGenerator::CheckSeed(L, 1);

    auto* randomGenerator = instance()->NewRandomGenerator();

    if (lua_gettop(L) > 0)
    {
        luax::CatchException(
            L, [&]() { randomGenerator->SetSeed(seed); },
            [&](bool error) {
                if (error)
                    randomGenerator->Release();
            });
    }

    luax::PushType(L, randomGenerator);
    randomGenerator->Release();

    return 1;
}

int Wrap_Math::NewTransform(lua_State* L)
{
    Transform* transform = nullptr;

    if (lua_isnoneornil(L, 1))
        transform = instance()->NewTransform();
    else
    {
        float x  = luaL_checknumber(L, 1);
        float y  = luaL_checknumber(L, 2);
        float a  = luaL_optnumber(L, 3, 0.0);
        float sx = luaL_optnumber(L, 4, 1.0);
        float sy = luaL_optnumber(L, 5, sx);
        float ox = luaL_optnumber(L, 6, 0.0);
        float oy = luaL_optnumber(L, 7, 0.0);
        float kx = luaL_optnumber(L, 8, 0.0);
        float ky = luaL_optnumber(L, 9, 0.0);

        transform = instance()->NewTransform(x, y, a, sx, sy, ox, oy, kx, ky);
    }

    luax::PushType(L, transform);
    transform->Release();

    return 1;
}

int Wrap_Math::NewBezierCurve(lua_State* L)
{
    std::vector<Vector2> points {};

    if (lua_istable(L, 1))
    {
        int length = luax::ObjectLength(L, 1);
        points.reserve(length / 2);

        for (int index = 1; index <= length; index += 2)
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
        int length = lua_gettop(L);
        points.reserve(length / 2);

        for (int index = 1; index <= length; index += 2)
        {
            Vector2 point {};
            point.x = luaL_checknumber(L, index);
            point.y = luaL_checknumber(L, index + 1);

            points.push_back(point);
        }
    }

    auto* bezierCurve = instance()->NewBezierCurve(points);

    luax::PushType(L, bezierCurve);
    bezierCurve->Release();

    return 1;
}

int Wrap_Math::Triangulate(lua_State* L)
{
    std::vector<Vector2> vertices;

    if (lua_istable(L, 1))
    {
        int top = (int)luax::ObjectLength(L, 1);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            lua_rawgeti(L, 1, i);
            lua_rawgeti(L, 1, i + 1);

            Vector2 v;
            v.x = luaL_checknumber(L, -2);
            v.y = luaL_checknumber(L, -1);

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
            Vector2 v;
            v.x = luaL_checknumber(L, i);
            v.y = luaL_checknumber(L, i + 1);

            vertices.push_back(v);
        }
    }

    if (vertices.size() < 3)
        return luaL_error(L, "Need at least 3 vertices to triangulate");

    std::vector<Math::Triangle> triangles;

    luax::CatchException(L, [&]() {
        if (vertices.size() == 3)
            triangles.push_back(Math::Triangle(vertices[0], vertices[1], vertices[2]));
        else
            triangles = Math::Triangulate(vertices);
    });

    lua_createtable(L, (int)triangles.size(), 0);

    for (int i = 0; i < (int)triangles.size(); ++i)
    {
        const Math::Triangle& tri = triangles[i];

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

int Wrap_Math::IsConvex(lua_State* L)
{
    std::vector<Vector2> vertices;

    if (lua_istable(L, 1))
    {
        int top = (int)luax::ObjectLength(L, 1);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            lua_rawgeti(L, 1, i);
            lua_rawgeti(L, 1, i + 1);

            Vector2 v;
            v.x = luaL_checknumber(L, -2);
            v.y = luaL_checknumber(L, -1);

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
            Vector2 v;
            v.x = luaL_checknumber(L, i);
            v.y = luaL_checknumber(L, i + 1);

            vertices.push_back(v);
        }
    }

    luax::PushBoolean(L, Math::IsConvex(vertices));

    return 1;
}

static int getGammaArguments(lua_State* L, float color[4])
{
    int components = 0;

    if (lua_istable(L, 1))
    {
        int length = (int)luax::ObjectLength(L, 1);

        for (int index = 1; index <= length && index <= 4; index++)
        {
            lua_rawgeti(L, 1, index);
            color[index - 1] = luax::CheckNumberClamped01(L, -1);
            components++;
        }

        lua_pop(L, components);
    }
    else
    {
        int length = lua_gettop(L);
        for (int index = 1; index <= length && index <= 4; index++)
        {
            color[index - 1] = luax::CheckNumberClamped01(L, index);
            components++;
        }
    }

    if (components == 0)
        luaL_checknumber(L, 1);

    return components;
}

int Wrap_Math::GammaToLinear(lua_State* L)
{
    float color[4] {};
    int components = getGammaArguments(L, color);

    for (int index = 0; index < components; index++)
    {
        if (index < 3)
            color[index] = Math::GammaToLinear(color[index]);

        lua_pushnumber(L, color[index]);
    }

    return components;
}

int Wrap_Math::LinearToGamma(lua_State* L)
{
    float color[4] {};
    int components = getGammaArguments(L, color);

    for (int index = 0; index < components; index++)
    {
        if (index < 3)
            color[index] = Math::LinearToGamma(color[index]);

        lua_pushnumber(L, color[index]);
    }

    return components;
}

int Wrap_Math::PerlinNoise(lua_State* L)
{
    int argc = std::clamp(lua_gettop(L), 1, 4);
    double args[4] {};

    for (int index = 0; index < argc; index++)
        args[index] = luaL_checknumber(L, index + 1);

    double value = 0.0;

    switch (argc)
    {
        case 1:
        {
            value = Math::PerlinNoise1(args[0]);
            break;
        }
        case 2:
        {
            value = Math::PerlinNoise2(args[0], args[1]);
            break;
        }
        case 3:
        {
            value = Math::PerlinNoise3(args[0], args[1], args[2]);
            break;
        }
        case 4:
        {
            value = Math::PerlinNoise4(args[0], args[1], args[2], args[3]);
            break;
        }
    }

    lua_pushnumber(L, value);

    return 1;
}

int Wrap_Math::SimplexNoise(lua_State* L)
{
    int argc = std::clamp(lua_gettop(L), 1, 4);
    double args[4] {};

    for (int index = 0; index < argc; index++)
        args[index] = luaL_checknumber(L, index + 1);

    double value = 0.0;

    switch (argc)
    {
        case 1:
        {
            value = Math::SimplexNoise1(args[0]);
            break;
        }
        case 2:
        {
            value = Math::SimplexNoise2(args[0], args[1]);
            break;
        }
        case 3:
        {
            value = Math::SimplexNoise3(args[0], args[1], args[2]);
            break;
        }
        case 4:
        {
            value = Math::SimplexNoise4(args[0], args[1], args[2], args[3]);
            break;
        }
    }

    lua_pushnumber(L, value);

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "_getRandomGenerator", Wrap_Math::GetRandomGenerator },
    { "gammaToLinear",       Wrap_Math::GammaToLinear      },
    { "isConvex",            Wrap_Math::IsConvex           },
    { "linearToGamma",       Wrap_Math::LinearToGamma      },
    { "newBezierCurve",      Wrap_Math::NewBezierCurve     },
    { "newRandomGenerator",  Wrap_Math::NewRandomGenerator },
    { "newTransform",        Wrap_Math::NewTransform       },
    { "perlinNoise",         Wrap_Math::PerlinNoise        },
    { "simplexNoise",        Wrap_Math::SimplexNoise       },
    { "triangulate",         Wrap_Math::Triangulate        }
};

static constexpr lua_CFunction types[] =
{
    Wrap_RandomGenerator::Register,
    Wrap_Transform::Register,
    Wrap_BezierCurve::Register,
    nullptr
};
// clang-format on

int Wrap_Math::Register(lua_State* L)
{
    auto* instance = instance();

    if (instance == nullptr)
        luax::CatchException(L, [&]() { instance = new Math(); });
    else
        instance()->Retain();

    WrappedModule wrappedModule {};
    wrappedModule.instance  = instance;
    wrappedModule.name      = "math";
    wrappedModule.type      = &Module::type;
    wrappedModule.functions = functions;
    wrappedModule.types     = types;

    int result = luax::RegisterModule(L, wrappedModule);

    luaL_loadbuffer(L, wrap_math_lua, sizeof(wrap_math_lua), "=[love \"wrap_math.lua\"]");
    lua_pushvalue(L, -2);
    lua_call(L, 1, 0);

    return result;
}
