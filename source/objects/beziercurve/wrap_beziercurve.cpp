#include <objects/beziercurve/wrap_beziercurve.hpp>

using namespace love;

BezierCurve* Wrap_BezierCurve::CheckBezierCurve(lua_State* L, int index)
{
    return luax::CheckType<BezierCurve>(L, index);
}

int Wrap_BezierCurve::GetDegree(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    lua_pushnumber(L, self->GetDegree());

    return 1;
}

int Wrap_BezierCurve::GetDerivative(lua_State* L)
{
    auto* self       = Wrap_BezierCurve::CheckBezierCurve(L, 1);
    auto* derivative = new BezierCurve(self->GetDerivative());

    luax::PushType(L, derivative);
    derivative->Release();

    return 1;
}

int Wrap_BezierCurve::GetControlPoint(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);
    int index  = luaL_checkinteger(L, 2);

    if (index > 0)
        index--;

    luax::CatchException(L, [&]() {
        Vector2 point = self->GetControlPoint(index);

        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.x);
    });

    return 2;
}

int Wrap_BezierCurve::SetControlPoint(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    int index = luaL_checkinteger(L, 2);

    float x = luaL_checknumber(L, 3);
    float y = luaL_checknumber(L, 4);

    if (index > 0)
        index--;

    luax::CatchException(L, [&]() { self->SetControlPoint(index, Vector2(x, y)); });

    return 0;
}

int Wrap_BezierCurve::InsertControlPoint(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    int index = luaL_optinteger(L, 4, -1);

    if (index > 0)
        index--;

    luax::CatchException(L, [&]() { self->InsertControlPoint(Vector2(x, y), index); });

    return 0;
}

int Wrap_BezierCurve::RemoveControlPoint(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);
    int index  = luaL_checkinteger(L, 2);

    if (index > 0)
        index--;

    luax::CatchException(L, [&]() { self->RemoveControlPoint(index); });

    return 0;
}

int Wrap_BezierCurve::GetControlPointCount(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    lua_pushinteger(L, self->GetControlPointCount());

    return 1;
}

int Wrap_BezierCurve::Translate(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->Translate(Vector2(x, y));

    return 0;
}

int Wrap_BezierCurve::Rotate(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    double phi = luaL_checknumber(L, 2);

    float x = luaL_optnumber(L, 3, 0);
    float y = luaL_optnumber(L, 4, 0);

    self->Rotate(phi, Vector2(x, y));

    return 0;
}

int Wrap_BezierCurve::Scale(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    double scale = luaL_checknumber(L, 2);

    float x = luaL_optnumber(L, 3, 0);
    float y = luaL_optnumber(L, 4, 0);

    self->Scale(scale, Vector2(x, y));

    return 0;
}

int Wrap_BezierCurve::Evaluate(lua_State* L)
{
    auto* self  = Wrap_BezierCurve::CheckBezierCurve(L, 1);
    double time = luaL_checknumber(L, 2);

    luax::CatchException(L, [&]() {
        Vector2 point = self->Evaluate(time);

        lua_pushnumber(L, point.x);
        lua_pushnumber(L, point.y);
    });

    return 2;
}

int Wrap_BezierCurve::GetSegment(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    double start = luaL_checknumber(L, 2);
    double end   = luaL_checknumber(L, 3);

    BezierCurve* segment = nullptr;
    luax::CatchException(L, [&]() { segment = self->GetSegment(start, end); });

    luax::PushType(L, segment);
    segment->Release();

    return 1;
}

int Wrap_BezierCurve::Render(lua_State* L)
{
    auto* self   = Wrap_BezierCurve::CheckBezierCurve(L, 1);
    int accuracy = luaL_optinteger(L, 2, 5);

    std::vector<Vector2> points {};
    luax::CatchException(L, [&]() { points = self->Render(accuracy); });

    lua_createtable(L, (int)points.size() * 2, 0);

    for (int index = 0; index < (int)points.size(); ++index)
    {
        lua_pushnumber(L, points[index].x);
        lua_rawseti(L, -2, 2 * index + 1);

        lua_pushnumber(L, points[index].y);
        lua_rawseti(L, -2, 2 * index + 2);
    }

    return 1;
}

int Wrap_BezierCurve::RenderSegment(lua_State* L)
{
    auto* self = Wrap_BezierCurve::CheckBezierCurve(L, 1);

    double start = luaL_checknumber(L, 2);
    double end   = luaL_checknumber(L, 3);
    int accuracy = luaL_optinteger(L, 4, 5);

    std::vector<Vector2> points {};
    luax::CatchException(L, [&]() { points = self->RenderSegment(start, end, accuracy); });

    lua_createtable(L, (int)points.size() * 2, 0);

    for (int index = 0; index < (int)points.size(); ++index)
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
    { "evaluate",             Wrap_BezierCurve::Evaluate             },
    { "getControlPoint",      Wrap_BezierCurve::GetControlPoint      },
    { "getControlPointCount", Wrap_BezierCurve::GetControlPointCount },
    { "getDegree",            Wrap_BezierCurve::GetDegree            },
    { "getDerivative",        Wrap_BezierCurve::GetDerivative        },
    { "getSegment",           Wrap_BezierCurve::GetSegment           },
    { "insertControlPoint",   Wrap_BezierCurve::InsertControlPoint   },
    { "removeControlPoint",   Wrap_BezierCurve::RemoveControlPoint   },
    { "render",               Wrap_BezierCurve::Render               },
    { "renderSegment",        Wrap_BezierCurve::RenderSegment        },
    { "rotate",               Wrap_BezierCurve::Rotate               },
    { "scale",                Wrap_BezierCurve::Scale                },
    { "setControlPoint",      Wrap_BezierCurve::SetControlPoint      },
    { "translate",            Wrap_BezierCurve::Translate            }
};
// clang-format on

int Wrap_BezierCurve::Register(lua_State* L)
{
    return luax::RegisterType(L, &BezierCurve::type, functions);
}
