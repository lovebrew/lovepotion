#include "modules/math/wrap_Transform.hpp"

using namespace love;

int Wrap_Transform::clone(lua_State* L)
{
    auto* self  = luax_checktransform(L, 1);
    auto* clone = self->clone();

    luax_pushtype(L, clone);
    clone->release();

    return 1;
}

int Wrap_Transform::inverse(lua_State* L)
{
    auto* self    = luax_checktransform(L, 1);
    auto* inverse = self->inverse();

    luax_pushtype(L, inverse);
    inverse->release();

    return 1;
}

int Wrap_Transform::apply(lua_State* L)
{
    auto* self  = luax_checktransform(L, 1);
    auto* other = luax_checktransform(L, 2);

    self->apply(other);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::isAffine2DTransform(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);

    luax_pushboolean(L, self->getMatrix().isAffine2DTransform());

    return 1;
}

int Wrap_Transform::translate(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);
    auto x     = static_cast<float>(luaL_checknumber(L, 2));
    auto y     = static_cast<float>(luaL_checknumber(L, 3));

    self->translate(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::rotate(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);
    auto angle = static_cast<float>(luaL_checknumber(L, 2));

    self->rotate(angle);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::scale(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);
    auto x     = static_cast<float>(luaL_checknumber(L, 2));
    auto y     = static_cast<float>(luaL_checknumber(L, 3));

    self->scale(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::shear(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);
    auto x     = static_cast<float>(luaL_checknumber(L, 2));
    auto y     = static_cast<float>(luaL_checknumber(L, 3));

    self->shear(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::reset(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);
    self->reset();

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::setTransformation(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);

    float x     = luaL_optnumber(L, 2, 0.0f);
    float y     = luaL_optnumber(L, 3, 0.0f);
    float angle = luaL_optnumber(L, 4, 0.0f);
    float sx    = luaL_optnumber(L, 5, 0.0f);
    float sy    = luaL_optnumber(L, 6, sx);
    float ox    = luaL_optnumber(L, 7, 0.0f);
    float oy    = luaL_optnumber(L, 8, 0.0f);
    float kx    = luaL_optnumber(L, 9, 0.0f);
    float ky    = luaL_optnumber(L, 10, 0.0f);

    self->setTransformation(x, y, angle, sx, sy, ox, oy, kx, ky);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::setMatrix(lua_State* L)
{
    auto* self  = luax_checktransform(L, 1);
    auto layout = Transform::MATRIX_ROW_MAJOR;

    int index = 2;
    if (lua_type(L, index) == LUA_TSTRING)
    {
        const char* layoutName = lua_tostring(L, index);
        if (!Transform::getConstant(layoutName, layout))
            return luax_enumerror(L, "matrix layout", Transform::MatrixLayouts, layoutName);

        index++;
    }

    float elements[16] {};
    luax_checkmatrix(L, index, layout, elements);

    self->setMatrix(Matrix4(elements));
    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::getMatrix(lua_State* L)
{
    auto* self            = luax_checktransform(L, 1);
    const float* elements = self->getMatrix().getElements();

    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
            lua_pushnumber(L, elements[column * 4 + row]);
    }

    return 16;
}

int Wrap_Transform::transformPoint(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);

    Vector2 point {};
    point.x = luaL_checknumber(L, 2);
    point.y = luaL_checknumber(L, 3);

    point = self->transformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Transform::inverseTransformPoint(lua_State* L)
{
    auto* self = luax_checktransform(L, 1);

    Vector2 point {};
    point.x = luaL_checknumber(L, 2);
    point.y = luaL_checknumber(L, 3);

    point = self->inverseTransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Transform::__mul(lua_State* L)
{
    auto* self  = luax_checktransform(L, 1);
    auto* other = luax_checktransform(L, 2);

    auto* result = new Transform(self->getMatrix() * other->getMatrix());

    luax_pushtype(L, result);
    result->release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",                 Wrap_Transform::clone                 },
    { "inverse",               Wrap_Transform::inverse               },
    { "apply",                 Wrap_Transform::apply                 },
    { "isAffine2DTransform",   Wrap_Transform::isAffine2DTransform   },
    { "translate",             Wrap_Transform::translate             },
    { "rotate",                Wrap_Transform::rotate                },
    { "scale",                 Wrap_Transform::scale                 },
    { "shear",                 Wrap_Transform::shear                 },
    { "reset",                 Wrap_Transform::reset                 },
    { "setTransformation",     Wrap_Transform::setTransformation     },
    { "setMatrix",             Wrap_Transform::setMatrix             },
    { "getMatrix",             Wrap_Transform::getMatrix             },
    { "transformPoint",        Wrap_Transform::transformPoint        },
    { "inverseTransformPoint", Wrap_Transform::inverseTransformPoint },
    { "__mul",                 Wrap_Transform::__mul                 }
};
// clang-format on

namespace love
{
    Transform* luax_checktransform(lua_State* L, int index)
    {
        return luax_checktype<Transform>(L, index, Transform::type);
    }

    void luax_checkmatrix(lua_State* L, int index, Transform::MatrixLayout layout, float elements[16])
    {
        const auto columnMajor = layout == Transform::MATRIX_COLUMN_MAJOR;

        if (lua_istable(L, index))
        {
            lua_rawgeti(L, index, 1);
            const auto tableOfTables = lua_istable(L, -1);
            lua_pop(L, 1);

            if (tableOfTables)
            {
                if (columnMajor)
                {
                    for (int column = 0; column < 4; column++)
                    {
                        lua_rawgeti(L, index, column + 1);

                        for (int row = 0; row < 4; row++)
                        {
                            lua_rawgeti(L, -(row + 1), row + 1);
                            elements[column * 4 + row] = luaL_checknumber(L, -1);
                        }

                        lua_pop(L, 4 + 1);
                    }
                }
                else
                {
                    for (int row = 0; row < 4; row++)
                    {
                        lua_rawgeti(L, index, row + 1);

                        for (int column = 0; column < 4; column++)
                        {
                            // The table has the matrix elements laid out in row-major
                            // order, but we need to store them column-major in memory.
                            lua_rawgeti(L, -(column + 1), column + 1);
                            elements[column * 4 + row] = luaL_checknumber(L, -1);
                        }

                        lua_pop(L, 4 + 1);
                    }
                }
            }
            else
            {
                if (columnMajor)
                {
                    for (int column = 0; column < 4; column++)
                    {
                        for (int row = 0; row < 4; row++)
                        {
                            lua_rawgeti(L, index, column * 4 + row + 1);
                            elements[column * 4 + row] = luaL_checknumber(L, -1);
                        }
                    }
                }
                else
                {
                    for (int column = 0; column < 4; column++)
                    {
                        for (int row = 0; row < 4; row++)
                        {
                            lua_rawgeti(L, index, row * 4 + column + 1);
                            elements[column * 4 + row] = luaL_checknumber(L, -1);
                        }
                    }
                }

                lua_pop(L, 16);
            }
        }
        else
        {
            if (columnMajor)
            {
                for (int i = 0; i < 16; i++)
                    elements[index] = luaL_checknumber(L, index + i);
            }
            else
            {
                for (int column = 0; column < 4; column++)
                {
                    for (int row = 0; row < 4; row++)
                        elements[column * 4 + row] = luaL_checknumber(L, row * 4 + column + index);
                }
            }
        }
    }

    int open_transform(lua_State* L)
    {
        return luax_register_type(L, &Transform::type, functions);
    }
} // namespace love
