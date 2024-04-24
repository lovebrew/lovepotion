#include "common/Exception.hpp"

#include "modules/math/wrap_BezierCurve.hpp"

#include <cmath>

using namespace love;

int Wrap_BezierCurve::getDegree(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    lua_pushinteger(L, self->getDegree());

    return 1;
}

int Wrap_BezierCurve::getDerivative(lua_State* L)
{
    auto* self       = luax_checkbeziercurve(L, 1);
    auto* derivative = new BezierCurve(self->getDerivative());

    luax_pushtype(L, derivative);
    derivative->release();

    return 1;
}

int Wrap_BezierCurve::getControlPoint(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    int index  = luaL_checkinteger(L, 2);

    if (index > 0)
        index--;

    luax_catchexcept(L, [&]() {
        Vector2 point = self->getControlPoint(index);
        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    });

    return 2;
}

int Wrap_BezierCurve::setControlPoint(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    int index  = luaL_checkinteger(L, 2);
    float x    = luaL_checknumber(L, 3);
    float y    = luaL_checknumber(L, 4);

    if (index > 0)
        index--;

    luax_catchexcept(L, [&]() { self->setControlPoint(index, Vector2(x, y)); });

    return 0;
}

int Wrap_BezierCurve::insertControlPoint(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    float x    = luaL_checknumber(L, 2);
    float y    = luaL_checknumber(L, 3);
    int index  = luaL_optinteger(L, 4, -1);

    if (index > 0)
        index--;

    luax_catchexcept(L, [&]() { self->insertControlPoint(Vector2(x, y), index); });

    return 0;
}

int Wrap_BezierCurve::removeControlPoint(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    int index  = luaL_checkinteger(L, 2);

    if (index > 0)
        index--;

    luax_catchexcept(L, [&]() { self->removeControlPoint(index); });

    return 0;
}

int Wrap_BezierCurve::getControlPointCount(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    lua_pushinteger(L, self->getControlPointCount());

    return 1;
}

int Wrap_BezierCurve::translate(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    float x    = luaL_checknumber(L, 2);
    float y    = luaL_checknumber(L, 3);

    luax_catchexcept(L, [&]() { self->translate(Vector2(x, y)); });

    return 0;
}

int Wrap_BezierCurve::rotate(lua_State* L)
{
    auto* self = luax_checkbeziercurve(L, 1);
    double phi = luaL_checknumber(L, 2);
    float x    = luaL_optnumber(L, 3, 0.0f);
    float y    = luaL_optnumber(L, 4, 0.0f);

    luax_catchexcept(L, [&]() { self->rotate(phi, Vector2(x, y)); });

    return 0;
}

int Wrap_BezierCurve::scale(lua_State* L)
{
    auto* self   = luax_checkbeziercurve(L, 1);
    double scale = luaL_checknumber(L, 2);
    float x      = luaL_optnumber(L, 3, 0.0f);
    float y      = luaL_optnumber(L, 4, 0.0f);

    luax_catchexcept(L, [&]() { self->scale(scale, Vector2(x, y)); });

    return 0;
}

int Wrap_BezierCurve::evaluate(lua_State* L)
{
    auto* self  = luax_checkbeziercurve(L, 1);
    double time = luaL_checknumber(L, 2);

    luax_catchexcept(L, [&]() {
        Vector2 point = self->evaluate(time);
        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    });

    return 2;
}

int Wrap_BezierCurve::getSegment(lua_State* L)
{
    auto* self   = luax_checkbeziercurve(L, 1);
    double start = luaL_checknumber(L, 2);
    double end   = luaL_checknumber(L, 3);

    BezierCurve* segment = nullptr;

    luax_catchexcept(L, [&]() { segment = self->getSegment(start, end); });

    luax_pushtype(L, segment);
    segment->release();

    return 1;
}

int Wrap_BezierCurve::render(lua_State* L)
{
    auto* self   = luax_checkbeziercurve(L, 1);
    int accuracy = luaL_optinteger(L, 2, 5);

    std::vector<Vector2> points {};
    luax_catchexcept(L, [&]() { points = self->render(accuracy); });

    lua_createtable(L, points.size() * 2, 0);
    for (size_t index = 0; index < points.size(); ++index)
    {
        lua_pushnumber(L, points[index].x);
        lua_rawseti(L, -2, 2 * index + 1);

        lua_pushnumber(L, points[index].y);
        lua_rawseti(L, -2, 2 * index + 2);
    }

    return 1;
}

int Wrap_BezierCurve::renderSegment(lua_State* L)
{
    auto* self   = luax_checkbeziercurve(L, 1);
    double start = luaL_checknumber(L, 2);
    double end   = luaL_checknumber(L, 3);
    int accuracy = luaL_optinteger(L, 4, 5);

    std::vector<Vector2> points {};
    luax_catchexcept(L, [&]() { points = self->renderSegment(start, end, accuracy); });

    lua_createtable(L, points.size() * 2, 0);
    for (size_t index = 0; index < points.size(); ++index)
    {
        lua_pushnumber(L, points[index].x);
        lua_rawseti(L, -2, 2 * index + 1);

        lua_pushnumber(L, points[index].y);
        lua_rawseti(L, -2, 2 * index + 2);
    }

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "getDegree",            Wrap_BezierCurve::getDegree            },
    { "getDerivative",        Wrap_BezierCurve::getDerivative        },
    { "getControlPoint",      Wrap_BezierCurve::getControlPoint      },
    { "setControlPoint",      Wrap_BezierCurve::setControlPoint      },
    { "insertControlPoint",   Wrap_BezierCurve::insertControlPoint   },
    { "removeControlPoint",   Wrap_BezierCurve::removeControlPoint   },
    { "getControlPointCount", Wrap_BezierCurve::getControlPointCount },
    { "translate",            Wrap_BezierCurve::translate            },
    { "rotate",               Wrap_BezierCurve::rotate               },
    { "scale",                Wrap_BezierCurve::scale                },
    { "evaluate",             Wrap_BezierCurve::evaluate             },
    { "getSegment",           Wrap_BezierCurve::getSegment           },
    { "render",               Wrap_BezierCurve::render               },
    { "renderSegment",        Wrap_BezierCurve::renderSegment        }
};
// clang-format on

namespace love
{
    BezierCurve* luax_checkbeziercurve(lua_State* L, int index)
    {
        return luax_checktype<BezierCurve>(L, index);
    }

    int open_beziercurve(lua_State* L)
    {
        return luax_register_type(L, &BezierCurve::type, functions);
    }
} // namespace love
