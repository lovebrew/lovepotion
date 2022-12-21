#include <objects/transform/wrap_transform.hpp>

using namespace love;

Transform* Wrap_Transform::CheckTransform(lua_State* L, int index)
{
    return luax::CheckType<Transform>(L, index);
}

int Wrap_Transform::Clone(lua_State* L)
{
    auto* self  = Wrap_Transform::CheckTransform(L, 1);
    auto* clone = self->Clone();

    luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_Transform::Inverse(lua_State* L)
{
    auto* self    = Wrap_Transform::CheckTransform(L, 1);
    auto* inverse = self->Inverse();

    luax::PushType(L, inverse);
    inverse->Release();

    return 1;
}

int Wrap_Transform::Apply(lua_State* L)
{
    auto* self  = Wrap_Transform::CheckTransform(L, 1);
    auto* other = Wrap_Transform::CheckTransform(L, 2);

    self->Apply(other);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::IsAffine2DTransform(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    luax::PushBoolean(L, self->GetMatrix().IsAffine2DTransform());

    return 1;
}

int Wrap_Transform::Translate(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->Translate(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Rotate(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    float angle = luaL_checknumber(L, 2);

    self->Rotate(angle);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Scale(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_optnumber(L, 3, x);

    self->Scale(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Shear(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->Shear(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Reset(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    self->Reset();

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::SetTransformation(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    float x  = luaL_optnumber(L, 2, 0.0);
    float y  = luaL_optnumber(L, 3, 0.0);
    float a  = luaL_optnumber(L, 4, 0.0);
    float sx = luaL_optnumber(L, 5, 1.0);
    float sy = luaL_optnumber(L, 6, sx);
    float ox = luaL_optnumber(L, 7, 0.0);
    float oy = luaL_optnumber(L, 8, 0.0);
    float kx = luaL_optnumber(L, 9, 0.0);
    float ky = luaL_optnumber(L, 10, 0.0);

    self->SetTransformation(x, y, a, sx, sy, ox, oy, kx, ky);

    lua_pushvalue(L, 1);

    return 1;
}

/* todo */
int Wrap_Transform::SetMatrix(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    bool columnMajor = false;
    int index        = 2;

    if (lua_type(L, index) == LUA_TSTRING)
    {
        const char* layoutString = lua_tostring(L, index);
        std::optional<Transform::MatrixLayout> layout;

        if (!(layout = Transform::matrixLayouts.Find(layoutString)))
            return luax::EnumError(L, "matrix layout", Transform::matrixLayouts, layoutString);

        columnMajor = (*layout == Transform::MATRIX_COLUMN_MAJOR);
        index++;
    }

    auto& matrix = self->GetMatrix();

    if (lua_istable(L, index))
    {
        lua_rawgeti(L, index, 1);
        bool isTableOfTables = lua_istable(L, -1);

        lua_pop(L, 1);

        if (isTableOfTables)
        {
            if (columnMajor)
            {
                for (int column = 0; column < 4; column++)
                {
                    lua_rawgeti(L, index, column + 1);

                    for (int row = 0; row < 4; row++)
                    {
                        lua_rawgeti(L, -(row + 1), row + 1);
                        matrix.Set(row, column, luaL_checknumber(L, -1));
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
                        lua_rawgeti(L, -(column + 1), column + 1);
                        matrix.Set(row, column, luaL_checknumber(L, -1));
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
                        matrix.Set(row, column, luaL_checknumber(L, -1));
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
                        matrix.Set(row, column, luaL_checknumber(L, -1));
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
            for (int column = 0; column < 4; column++)
            {
                for (int row = 0; row < 4; row++)
                    matrix.Set(row, column, luaL_checknumber(L, column * 4 + row + index));
            }
        }
        else
        {
            for (int column = 0; column < 4; column++)
            {
                for (int row = 0; row < 4; row++)
                    matrix.Set(row, column, luaL_checknumber(L, row * 4 + column + index));
            }
        }
    }

    if (columnMajor)
        matrix.Transpose();

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::GetMatrix(lua_State* L)
{
    auto* self           = Wrap_Transform::CheckTransform(L, 1);
    const auto& elements = self->GetMatrix();

    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
            lua_pushnumber(L, elements.Get(row, column));
    }

    return 16;
}

int Wrap_Transform::TransformPoint(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    Vector2 point {};

    point.x = luaL_checknumber(L, 2);
    point.y = luaL_checknumber(L, 3);

    point = self->TransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Transform::InverseTransformPoint(lua_State* L)
{
    auto* self = Wrap_Transform::CheckTransform(L, 1);

    Vector2 point {};

    point.x = luaL_checknumber(L, 2);
    point.y = luaL_checknumber(L, 3);

    point = self->InverseTransformPoint(point);

    lua_pushnumber(L, point.x);
    lua_pushnumber(L, point.y);

    return 2;
}

int Wrap_Transform::__Mul(lua_State* L)
{
    auto* self  = Wrap_Transform::CheckTransform(L, 1);
    auto* other = Wrap_Transform::CheckTransform(L, 2);

    auto* result = new Transform(self->GetMatrix() * other->GetMatrix());

    luax::PushType(L, result);
    result->Release();

    return 1;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "__mul",                 Wrap_Transform::__Mul                 },
    { "apply",                 Wrap_Transform::Apply                 },
    { "clone",                 Wrap_Transform::Clone                 },
    { "getMatrix",             Wrap_Transform::GetMatrix             },
    { "inverse",               Wrap_Transform::Inverse               },
    { "inverseTransformPoint", Wrap_Transform::InverseTransformPoint },
    { "isAffine2DTransform",   Wrap_Transform::IsAffine2DTransform   },
    { "reset",                 Wrap_Transform::Reset                 },
    { "rotate",                Wrap_Transform::Rotate                },
    { "setMatrix",             Wrap_Transform::SetMatrix             },
    { "setTransformation",     Wrap_Transform::SetTransformation     },
    { "scale",                 Wrap_Transform::Scale                 },
    { "shear",                 Wrap_Transform::Shear                 },
    { "transformPoint",        Wrap_Transform::TransformPoint        },
    { "translate",             Wrap_Transform::Translate             }
};
// clang-format on

int Wrap_Transform::Register(lua_State* L)
{
    return luax::RegisterType(L, &Transform::type, functions);
}
