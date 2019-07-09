#include "common/runtime.h"
#include "modules/mod_math.h"

#include "common/vector2.h"
#include "common/triangle.h"

#include "objects/randomgenerator/randomgenerator.h"
#include "objects/randomgenerator/wrap_randomgenerator.h"

#include <noise1234.h>
#include <simplexnoise1234.h>

#include <time.h>

// Our own personal RNG!
RandomGenerator rng;

//love.math.setRandomSeed
int Math::SetRandomSeed(lua_State * L)
{
    RandomGenerator::Seed s;
    if (lua_gettop(L) >= 2)
    {
        s.b32.low = checkrandomseed_part<u32>(L, 1);
        s.b32.high = checkrandomseed_part<u32>(L, 2);
    }
    else
        s.b64 = checkrandomseed_part<u64>(L, 1);

    rng.setSeed(s);

    return 0;
}

//love.math.setRandomState
int Math::SetRandomState(lua_State * L)
{
    rng.setState(luaL_checkstring(L, 1));

    return 0;
}


//love.math.random
int Math::Random(lua_State * L)
{
    double n = 0;
    int top = lua_gettop(L);

    if (top > 0)
    {
        if (top >= 2)
        {
            double low = luaL_checknumber(L, 1);
            double high = luaL_checknumber(L, 2);

            n = rng.random(low, high);
        }
        else
        {
            double high = luaL_checknumber(L, 1);
            
            n = rng.random(high);
        }
    }
    else // No args, just a regular 0 to 1 RNG
    {
        n = rng.random();
    }
    
    lua_pushnumber(L, n);

    return 1;
}

//love.math.randomNormal
int Math::RandomNormal(lua_State * L)
{
    double stddev = luaL_optnumber(L, 1, 1.0);
    double mean = luaL_optnumber(L, 2, 0.0);

    double r = rng.randomNormal(stddev);

    lua_pushnumber(L, r + mean);

    return 1;
}

//love.math.getRandomSeed
int Math::GetRandomSeed(lua_State * L)
{
    RandomGenerator::Seed s = rng.getSeed();

    lua_pushnumber(L, s.b32.low);
    lua_pushnumber(L, s.b32.high);

    return 2;
}

//love.math.getRandomState
int Math::GetRandomState(lua_State * L)
{
    string s = rng.getState();

    lua_pushlstring(L, s.c_str(), s.length());

    return 1;
}

//love.math.gammaToLinear
int Math::GammaToLinear(lua_State * L)
{
    float color[4];
    int numcomponents = getGammaArgs(L, color);

    for (int i = 0; i < numcomponents; i++)
    {
        if (i < 3) // Alpha is always linear.
            color[i] = convertGammaToLinear(color[i]);
        lua_pushnumber(L, color[i]);
    }

    return numcomponents;
}

//love.math.linearToGamma
int Math::LinearToGamma(lua_State * L)
{
    float color[4];
    int numcomponents = getGammaArgs(L, color);

    for (int i = 0; i < numcomponents; i++)
    {
        if (i < 3) // Alpha is always linear.
            color[i] = convertLinearToGamma(color[i]);
        lua_pushnumber(L, color[i]);
    }

    return numcomponents;
}

//love.math.isConvex
int Math::IsConvex(lua_State * L)
{
    vector<Vector2> vertices;
    if (lua_istable(L, 1))
    {
        int top = lua_objlen(L, 1);
        vertices.reserve(top / 2);
        for (int i = 1; i <= top; i += 2)
        {
            lua_rawgeti(L, 1, i);
            lua_rawgeti(L, 1, i+1);

            Vector2 v;
            v.x = (float) luaL_checknumber(L, -2);
            v.y = (float) luaL_checknumber(L, -1);
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
            v.x = (float) luaL_checknumber(L, i);
            v.y = (float) luaL_checknumber(L, i+1);
            vertices.push_back(v);
        }
    }

    if (vertices.size() < 3)
    {
        lua_pushboolean(L, false);

        return 1;
    }

    // a polygon is convex if all corners turn in the same direction
    // turning direction can be determined using the cross-product of
    // the forward difference vectors
    size_t i = vertices.size() - 2, j = vertices.size() - 1, k = 0;
    Vector2 p(vertices[j] - vertices[i]);
    Vector2 q(vertices[k] - vertices[j]);
    float winding = Vector2::cross(p, q);

    while (k+1 < vertices.size())
    {
        i = j; j = k; k++;
        p = vertices[j] - vertices[i];
        q = vertices[k] - vertices[j];

        if (Vector2::cross(p, q) * winding < 0)
        {
            lua_pushboolean(L, false);

            return 1;
        }
    }

    lua_pushboolean(L, true);

    return 1;
}

//love.math.noise
int Math::Noise(lua_State * L)
{
    int nargs = min(max(lua_gettop(L), 1), 4);
    float args[4];

    for (int i = 0; i < nargs; i++)
        args[i] = (float)luaL_checknumber(L, i + 1);

    float val = 0.0f;

    switch (nargs) // dimensions of noise vector
    {
        case 1:
            val = SimplexNoise1234::noise(args[0]) * 0.5f + 0.5f;
            break;
        case 2:
            val = SimplexNoise1234::noise(args[0], args[1]) * 0.5f + 0.5f;
            break;

        // Perlin noise is used instead of Simplex noise in the 3D and 4D cases to avoid patent issues.

        case 3:
            val = Noise1234::noise(args[0], args[1], args[2]) * 0.5f + 0.5f;
            break;
        case 4:
            val = Noise1234::noise(args[0], args[1], args[2], args[3]) * 0.5f + 0.5f;
            break;
    }

    lua_pushnumber(L, val);

    return 1;
}

//love.math.triangulate
int Math::Triangulate(lua_State * L)
{
    vector<Vector2> vertices;
    if (lua_istable(L, 1))
    {
        int top = (int) lua_objlen(L, 1);
        vertices.reserve(top / 2);

        for (int i = 1; i <= top; i += 2)
        {
            lua_rawgeti(L, 1, i);
            lua_rawgeti(L, 1, i+1);

            Vector2 v;
            v.x = (float) luaL_checknumber(L, -2);
            v.y = (float) luaL_checknumber(L, -1);
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
            v.x = (float) luaL_checknumber(L, i);
            v.y = (float) luaL_checknumber(L, i+1);
            vertices.push_back(v);
        }
    }

    if (vertices.size() < 3)
        return luaL_error(L, "Need at least 3 vertices to triangulate");
    else if (vertices.size() == 3)
    {
        // push the single triangle
        // make the table to store the table that stores the points
        lua_createtable(L, 6, 0);

        // store the points in the sub-table
        lua_createtable(L, 6, 0);

        lua_pushnumber(L, vertices[0].x);
        lua_rawseti(L, -2, 1);

        lua_pushnumber(L, vertices[0].y);
        lua_rawseti(L, -2, 2);

        lua_pushnumber(L, vertices[1].x);
        lua_rawseti(L, -2, 3);

        lua_pushnumber(L, vertices[1].y);
        lua_rawseti(L, -2, 4);

        lua_pushnumber(L, vertices[2].x);
        lua_rawseti(L, -2, 5);

        lua_pushnumber(L, vertices[2].y);
        lua_rawseti(L, -2, 6);

        // put the table into the table to be pushed
        lua_rawseti(L, -2, 1);

        return 1;
    }
    else
    {
        // collect list of connections and record leftmost item to check if the polygon
        // has the expected winding
        vector<size_t> next_idx(vertices.size()), prev_idx(vertices.size());

        size_t idx_lm = 0;

        for (size_t i = 0; i < vertices.size(); ++i)
        {
            const Vector2 &lm = vertices[idx_lm], &p = vertices[i];

            if (p.x < lm.x || (p.x == lm.x && p.y < lm.y))
                idx_lm = i;

            next_idx[i] = i+1;
            prev_idx[i] = i-1;
        }

        next_idx[next_idx.size()-1] = 0;
        prev_idx[0] = prev_idx.size()-1;

        // check if the polygon has the expected winding and reverse polygon if needed
        if (!isOrientedCCW(vertices[prev_idx[idx_lm]], vertices[idx_lm], vertices[next_idx[idx_lm]]))
            next_idx.swap(prev_idx);

        // collect list of concave polygons
        list<const Vector2 *> concave_vertices;
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            if (!isOrientedCCW(vertices[prev_idx[i]], vertices[i], vertices[next_idx[i]]))
                concave_vertices.push_back(&vertices[i]);
        }

        // triangulation according to kong
        vector<Triangle> triangles;
        size_t n_vertices = vertices.size();
        size_t current = 1, skipped = 0, next, prev;

        while (n_vertices > 3)
        {
            next = next_idx[current];
            prev = prev_idx[current];

            const Vector2 &a = vertices[prev], &b = vertices[current], &c = vertices[next];
            if (isEar(a,b,c, concave_vertices))
            {
                triangles.push_back(Triangle(a,b,c));
                next_idx[prev] = next;
                prev_idx[next] = prev;
                concave_vertices.remove(&b);
                --n_vertices;
                skipped = 0;
            }
            else if (++skipped > n_vertices)
                luaL_error(L, "Cannot triangulate polygon.");
            
            current = next;
        }

        next = next_idx[current];
        prev = prev_idx[current];

        triangles.push_back(Triangle(vertices[prev], vertices[current], vertices[next]));

        // Make triangles into a single table to return
        lua_createtable(L, (int) triangles.size(), 0);
        for (int i = 0; i < (int) triangles.size(); ++i)
        {
            const Triangle &tri = triangles[i];

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

            lua_rawseti(L, -2, i+1);
        }
    }

    return 1;
}


//Helper functions
float Math::convertGammaToLinear(float c)
{
    if (c <= 0.04045f)
        return c / 12.92f;
    else
        return powf((c + 0.055f) / 1.055f, 2.4f);
}

float Math::convertLinearToGamma(float c)
{
    if (c <= 0.0031308f)
        return c * 12.92f;
    else
        return 1.055f * powf(c, 1.0f / 2.4f) - 0.055f;
}

int Math::getGammaArgs(lua_State * L, float color[4])
{
    int numcomponents = 0;

    if (lua_istable(L, 1))
    {
        int n = (int) lua_objlen(L, 1);
        for (int i = 1; i <= n && i <= 4; i++)
        {
            lua_rawgeti(L, 1, i);
            color[i - 1] = (float) min(max(luaL_checknumber(L, -1), 0.0), 1.0);
            numcomponents++;
        }

        lua_pop(L, numcomponents);
    }
    else
    {
        int n = lua_gettop(L);
        for (int i = 1; i <= n && i <= 4; i++)
        {
            color[i - 1] = (float) min(max(luaL_checknumber(L, i), 0.0), 1.0);
            numcomponents++;
        }
    }

    if (numcomponents == 0)
        luaL_checknumber(L, 1);

    return numcomponents;
}

// check if an angle is oriented counter clockwise
bool Math::isOrientedCCW(const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    return ((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x)) >= 0;
}

// check if a and b are on the same side of the line c->d
bool Math::onSameSide(const Vector2 &a, const Vector2 &b, const Vector2 &c, const Vector2 &d)
{
    float px = d.x - c.x, py = d.y - c.y;
    float l = px * (a.y - c.y) - py * (a.x - c.x);
    float m = px * (b.y - c.y) - py * (b.x - c.x);

    return l * m >= 0;
}

// checks is p is contained in the triangle abc
bool Math::pointInTriangle(const Vector2 &p, const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    return onSameSide(p,a, b,c) && onSameSide(p,b, a,c) && onSameSide(p,c, a,b);
}

// checks if any vertex in `vertices' is in the triangle abc.
bool Math::anyPointInTriangle(const list<const Vector2 *> &vertices, const Vector2 &a, const Vector2 &b, const Vector2 &c)
{
    for (const Vector2 *p : vertices)
    {
        if ((p != &a) && (p != &b) && (p != &c) && pointInTriangle(*p, a,b,c))
            return true;
    }

    return false;
}

bool Math::isEar(const Vector2 &a, const Vector2 &b, const Vector2 &c, const list<const Vector2 *> &vertices)
{
    return isOrientedCCW(a,b,c) && !anyPointInTriangle(vertices, a,b,c);
}


int Math::Register(lua_State * L)
{
    // The module's random generator is always seeded with 
    RandomGenerator::Seed s;
    s.b64 = (u64) time(nullptr);
    rng.setSeed(s);

    luaL_Reg reg[] = 
    {
        { "gammaToLinear",      GammaToLinear      },
        { "getRandomSeed",      GetRandomSeed      },
        { "getRandomState",     GetRandomState     },
        { "isConvex",           IsConvex           },
        { "linearToGamma",      LinearToGamma      },
        // { "newBezierCurve",     beziercurveNew     },
        { "newRandomGenerator", randomgeneratorNew },
        // { "newTransform",       transformNew       },
        { "noise",              Noise              },
        { "random",             Random             },
        { "randomNormal",       RandomNormal       },
        { "setRandomSeed",      SetRandomSeed      },
        { "setRandomState",     SetRandomState     },
        { "triangulate",        Triangulate        },
        { 0, 0 }
    };

    luaL_newlib(L, reg);
    
    return 1;
}
