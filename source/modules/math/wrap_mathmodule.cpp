#include "modules/math/wrap_mathmodule.h"

#include "wrap_math_lua.h"

using namespace love;

#define instance() (Module::GetInstance<Math>(Module::M_MATH))

/**
 * http://en.wikipedia.org/wiki/SRGB#The_reverse_transformation
 **/
float love::GammaToLinear(float c)
{
    if (c <= 0.04045f)
        return c / 12.92f;
    else
        return powf((c + 0.055f) / 1.055f, 2.4f);
}

/**
 * http://en.wikipedia.org/wiki/SRGB#The_forward_transformation_.28CIE_xyY_or_CIE_XYZ_to_sRGB.29
 **/
float love::LinearToGamma(float c)
{
    if (c <= 0.0031308f)
        return c * 12.92f;
    else
        return 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
}

int Wrap_Math::GetRandomGenerator(lua_State* L)
{
    RandomGenerator* randomGenerator = instance()->GetRandomGenerator();

    Luax::PushType(L, randomGenerator);

    return 1;
}

int Wrap_Math::NewTransform(lua_State* L)
{
    Transform* transform = nullptr;

    if (lua_isnoneornil(L, 1))
        transform = instance()->NewTransform();
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

        transform = instance()->NewTransform(x, y, a, sx, sy, ox, oy, kx, ky);
    }

    Luax::PushType(L, transform);
    transform->Release();

    return 1;
}

static int GetGammaArgs(lua_State* L, float color[4])
{
    int numcomponents = 0;

    if (lua_istable(L, 1))
    {
        int n = lua_objlen(L, 1);
        for (int i = 1; i <= n && i <= 4; i++)
        {
            lua_rawgeti(L, 1, i);
            color[i - 1] = Luax::CheckNumberClamped01(L, -1);
            numcomponents++;
        }

        lua_pop(L, numcomponents);
    }
    else
    {
        int n = lua_gettop(L);
        for (int i = 1; i <= n && i <= 4; i++)
        {
            color[i - 1] = Luax::CheckNumberClamped01(L, i);
            numcomponents++;
        }
    }

    if (numcomponents == 0)
        luaL_checknumber(L, 1);

    return numcomponents;
}

int Wrap_Math::GammaToLinear(lua_State* L)
{
    float color[4];
    int numcomponents = GetGammaArgs(L, color);

    for (int i = 0; i < numcomponents; i++)
    {
        // Alpha should always be linear.
        if (i < 3)
            color[i] = love::GammaToLinear(color[i]);

        lua_pushnumber(L, color[i]);
    }

    return numcomponents;
}

int Wrap_Math::LinearToGamma(lua_State* L)
{
    float color[4];
    int numcomponents = GetGammaArgs(L, color);

    for (int i = 0; i < numcomponents; i++)
    {
        // Alpha should always be linear.
        if (i < 3)
            color[i] = love::LinearToGamma(color[i]);

        lua_pushnumber(L, color[i]);
    }

    return numcomponents;
}

int Wrap_Math::IsConvex(lua_State* L)
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
            v.y = (float)luaL_checknumber(L, i + 1);

            vertices.push_back(v);
        }
    }

    lua_pushboolean(L, instance()->IsConvex(vertices));

    return 1;
}

int Wrap_Math::NewRandomGenerator(lua_State* L)
{
    RandomGenerator::Seed seed;

    if (lua_gettop(L) > 0)
        seed = Wrap_RandomGenerator::CheckRandomSeed(L, 1);

    RandomGenerator* rng = instance()->NewRandomGenerator();

    if (lua_gettop(L) > 0)
    {
        bool shouldError = false;

        try
        {
            rng->SetSeed(seed);
        }
        catch (love::Exception& e)
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

int Wrap_Math::Noise(lua_State* L)
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

int Wrap_Math::Triangulate(lua_State* L)
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
            v.x = (float)luaL_checknumber(L, -2);
            v.y = (float)luaL_checknumber(L, -1);

            vertices.push_back(v);

            lua_pop(L, 2);
        }
    }
    else
    {
        int top = (int)lua_gettop(L);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            Vector2 v;
            v.x = (float)luaL_checknumber(L, i);
            v.y = (float)luaL_checknumber(L, i + 1);

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

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "_getRandomGenerator", Wrap_Math::GetRandomGenerator },
    { "gammaToLinear",       Wrap_Math::GammaToLinear      },
    { "isConvex",            Wrap_Math::IsConvex           },
    { "linearToGamma",       Wrap_Math::LinearToGamma      },
    { "newRandomGenerator",  Wrap_Math::NewRandomGenerator },
    { "newTransform",        Wrap_Math::NewTransform       },
    { "noise",               Wrap_Math::Noise              },
    { "triangulate",         Wrap_Math::Triangulate        },
    { 0,                     0                             }
};

static constexpr lua_CFunction types[] =
{
    Wrap_RandomGenerator::Register,
    Wrap_Transform::Register,
    nullptr
};
// clang-format on

int Wrap_Math::Register(lua_State* L)
{
    Math* instance = instance();

    if (instance == nullptr)
        Luax::CatchException(L, [&]() { instance = new Math(); });
    else
        instance->Retain();

    WrappedModule wrappedModule;

    wrappedModule.instance  = instance;
    wrappedModule.name      = "math";
    wrappedModule.functions = functions;
    wrappedModule.type      = &Module::type;
    wrappedModule.types     = types;

    int ret = Luax::RegisterModule(L, wrappedModule);

    luaL_loadbuffer(L, (const char*)wrap_math_lua, wrap_math_lua_size, "wrap_math.lua");
    lua_pushvalue(L, -2);
    lua_call(L, 1, 0);

    return ret;
}
