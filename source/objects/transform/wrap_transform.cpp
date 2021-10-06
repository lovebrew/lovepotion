#include "objects/transform/wrap_transform.h"

using namespace love;

int Wrap_Transform::Clone(lua_State* L)
{
    Transform* self  = Wrap_Transform::CheckTransform(L, 1);
    Transform* clone = self->Clone();

    Luax::PushType(L, clone);
    clone->Release();

    return 1;
}

int Wrap_Transform::Inverse(lua_State* L)
{
    Transform* self    = Wrap_Transform::CheckTransform(L, 1);
    Transform* inverse = self->Inverse();

    Luax::PushType(L, inverse);
    inverse->Release();

    return 1;
}

int Wrap_Transform::Apply(lua_State* L)
{
    Transform* self  = Wrap_Transform::CheckTransform(L, 1);
    Transform* other = Wrap_Transform::CheckTransform(L, 2);

    self->Apply(other);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::IsAffine2DTransform(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);
    lua_pushboolean(L, self->GetMatrix().IsAffine2DTransform());

    return 1;
}

int Wrap_Transform::Translate(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->Translate(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Rotate(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);
    float angle     = luaL_checknumber(L, 2);

    self->Rotate(angle);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Scale(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->Scale(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Shear(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);

    float x = luaL_checknumber(L, 2);
    float y = luaL_checknumber(L, 3);

    self->Shear(x, y);

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::Reset(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);
    self->Reset();

    lua_pushvalue(L, 1);

    return 1;
}

int Wrap_Transform::SetTransformation(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);

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

int Wrap_Transform::SetMatrix(lua_State* L)
{

#if defined(__SWITCH__)
    Transform* self = Wrap_Transform::CheckTransform(L, 1);

    bool columnmajor = false;

    int idx = 2;
    if (lua_type(L, idx) == LUA_TSTRING)
    {
        const char* layoutstr = lua_tostring(L, idx);
        Transform::MatrixLayout layout;
        if (!Transform::GetConstant(layoutstr, layout))
            return Luax::EnumError(L, "matrix layout", Transform::GetConstants(layout), layoutstr);

        columnmajor = (layout == Transform::MATRIX_COLUMN_MAJOR);
        idx++;
    }

    float elements[16];

    if (lua_istable(L, idx))
    {
        lua_rawgeti(L, idx, 1);
        bool tableoftables = lua_istable(L, -1);
        lua_pop(L, 1);

        if (tableoftables)
        {
            if (columnmajor)
            {
                for (int column = 0; column < 4; column++)
                {
                    lua_rawgeti(L, idx, column + 1);

                    for (int row = 0; row < 4; row++)
                    {
                        lua_rawgeti(L, -(row + 1), row + 1);
                        elements[column * 4 + row] = (float)luaL_checknumber(L, -1);
                    }

                    lua_pop(L, 4 + 1);
                }
            }
            else
            {
                for (int row = 0; row < 4; row++)
                {
                    lua_rawgeti(L, idx, row + 1);

                    for (int column = 0; column < 4; column++)
                    {
                        // The table has the matrix elements laid out in row-major
                        // order, but we need to store them column-major in memory.
                        lua_rawgeti(L, -(column + 1), column + 1);
                        elements[column * 4 + row] = (float)luaL_checknumber(L, -1);
                    }

                    lua_pop(L, 4 + 1);
                }
            }
        }
        else
        {
            if (columnmajor)
            {
                for (int column = 0; column < 4; column++)
                {
                    for (int row = 0; row < 4; row++)
                    {
                        lua_rawgeti(L, idx, column * 4 + row + 1);
                        elements[column * 4 + row] = (float)luaL_checknumber(L, -1);
                    }
                }
            }
            else
            {
                for (int column = 0; column < 4; column++)
                {
                    for (int row = 0; row < 4; row++)
                    {
                        // The table has the matrix elements laid out in row-major
                        // order, but we need to store them column-major in memory.
                        lua_rawgeti(L, idx, row * 4 + column + 1);
                        elements[column * 4 + row] = (float)luaL_checknumber(L, -1);
                    }
                }
            }

            lua_pop(L, 16);
        }
    }
    else
    {
        if (columnmajor)
        {
            for (int i = 0; i < 16; i++)
                elements[i] = (float)luaL_checknumber(L, idx + i);
        }
        else
        {
            for (int column = 0; column < 4; column++)
            {
                for (int row = 0; row < 4; row++)
                    elements[column * 4 + row] = (float)luaL_checknumber(L, row * 4 + column + idx);
            }
        }
    }

    self->SetMatrix(Matrix4(elements));
    lua_pushvalue(L, 1);

    return 1;
#endif
    return 0;
}

int Wrap_Transform::GetMatrix(lua_State* L)
{
#if defined(__SWITCH__)
    Transform* self          = Wrap_Transform::CheckTransform(L, 1);
    const Elements& elements = self->GetMatrix().GetElements();

    for (int row = 0; row < 4; row++)
    {
        for (int column = 0; column < 4; column++)
            lua_pushnumber(L, elements[column * 4 + row]);
    }

    return 16;
#endif
    return 0;
}

int Wrap_Transform::TransformPoint(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);
    love::Vector2 p;

    p.x = luaL_checknumber(L, 2);
    p.y = luaL_checknumber(L, 3);

    p = self->TransformPoint(p);

    lua_pushnumber(L, p.x);
    lua_pushnumber(L, p.y);

    return 2;
}

int Wrap_Transform::InverseTransformPoint(lua_State* L)
{
    Transform* self = Wrap_Transform::CheckTransform(L, 1);
    love::Vector2 p;

    p.x = (float)luaL_checknumber(L, 2);
    p.y = (float)luaL_checknumber(L, 3);

    p = self->InverseTransformPoint(p);

    lua_pushnumber(L, p.x);
    lua_pushnumber(L, p.y);

    return 2;
}

int Wrap_Transform::_Mul(lua_State* L)
{
    Transform* t1 = Wrap_Transform::CheckTransform(L, 1);
    Transform* t2 = Wrap_Transform::CheckTransform(L, 2);
    Transform* t3 = new Transform(t1->GetMatrix() * t2->GetMatrix());

    Luax::PushType(L, t3);

    t3->Release();

    return 1;
}

Transform* Wrap_Transform::CheckTransform(lua_State* L, int index)
{
    return Luax::CheckType<Transform>(L, index);
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "clone",                 Wrap_Transform::Clone                 },
    { "inverse",               Wrap_Transform::Inverse               },
    { "apply",                 Wrap_Transform::Apply                 },
    { "isAffine2DTransform",   Wrap_Transform::IsAffine2DTransform   },
    { "translate",             Wrap_Transform::Translate             },
    { "rotate",                Wrap_Transform::Rotate                },
    { "scale",                 Wrap_Transform::Scale                 },
    { "shear",                 Wrap_Transform::Shear                 },
    { "reset",                 Wrap_Transform::Reset                 },
    { "setTransformation",     Wrap_Transform::SetTransformation     },
    { "setMatrix",             Wrap_Transform::SetMatrix             },
    { "getMatrix",             Wrap_Transform::GetMatrix             },
    { "transformPoint",        Wrap_Transform::TransformPoint        },
    { "inverseTransformPoint", Wrap_Transform::InverseTransformPoint },
    { "__mul",                 Wrap_Transform::_Mul                  },
    { 0,                       0                                     }
};
// clang-format on

int Wrap_Transform::Register(lua_State* L)
{
    return Luax::RegisterType(L, &Transform::type, functions, nullptr);
}
