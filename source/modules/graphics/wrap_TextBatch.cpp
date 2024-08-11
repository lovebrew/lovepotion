#include "modules/graphics/wrap_TextBatch.hpp"

#include "modules/graphics/wrap_Font.hpp"
#include "modules/math/wrap_Transform.hpp"

using namespace love;

int Wrap_TextBatch::set(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    std::vector<ColoredString> newText {};
    luax_checkcoloredstring(L, 2, newText);

    luax_catchexcept(L, [&]() { self->set(newText); });

    return 0;
}

int Wrap_TextBatch::setf(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    float limit = luaL_checknumber(L, 3);

    FontBase::AlignMode align;
    const char* name = luaL_checkstring(L, 4);

    if (!FontBase::getConstant(name, align))
        return luax_enumerror(L, "align mode", FontBase::AlignModes, name);

    std::vector<ColoredString> newText {};
    luax_checkcoloredstring(L, 2, newText);

    luax_catchexcept(L, [&]() { self->set(newText, limit, align); });

    return 0;
}

int Wrap_TextBatch::add(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    int index = 0;

    std::vector<ColoredString> text {};
    luax_checkcoloredstring(L, 2, text);

    if (luax_istype(L, 3, Transform::type))
    {
        auto* transform = luax_totype<Transform>(L, 3);
        luax_catchexcept(L, [&]() { index = self->add(text, transform->getMatrix()); });
    }
    else
    {
        float x = luaL_optnumber(L, 3, 0.0f);
        float y = luaL_optnumber(L, 4, 0.0f);
        float a = luaL_optnumber(L, 5, 0.0f);

        float sx = luaL_optnumber(L, 6, 1.0f);
        float sy = luaL_optnumber(L, 7, sx);

        float ox = luaL_optnumber(L, 8, 0.0f);
        float oy = luaL_optnumber(L, 9, 0.0f);

        float kx = luaL_optnumber(L, 10, 0.0f);
        float ky = luaL_optnumber(L, 11, 0.0f);

        Matrix4 matrix(x, y, a, sx, sy, ox, oy, kx, ky);
        luax_catchexcept(L, [&]() { index = self->add(text, matrix); });
    }

    lua_pushinteger(L, index + 1);

    return 1;
}

int Wrap_TextBatch::addf(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    int index = 0;

    std::vector<ColoredString> newText {};
    luax_checkcoloredstring(L, 2, newText);

    float wrap = luaL_checknumber(L, 3);

    auto align       = FontBase::ALIGN_MAX_ENUM;
    const char* name = luaL_checkstring(L, 4);

    if (!FontBase::getConstant(name, align))
        return luax_enumerror(L, "align mode", FontBase::AlignModes, name);

    if (luax_istype(L, 5, Transform::type))
    {
        auto* transform = luax_totype<Transform>(L, 5);
        luax_catchexcept(L, [&]() { index = self->addf(newText, wrap, align, transform->getMatrix()); });
    }
    else
    {
        float x  = luaL_optnumber(L, 5, 0.0f);
        float y  = luaL_optnumber(L, 6, 0.0f);
        float a  = luaL_optnumber(L, 7, 0.0f);
        float sx = luaL_optnumber(L, 8, 1.0f);
        float sy = luaL_optnumber(L, 9, sx);
        float ox = luaL_optnumber(L, 10, 0.0f);
        float oy = luaL_optnumber(L, 11, 0.0f);
        float kx = luaL_optnumber(L, 12, 0.0f);
        float ky = luaL_optnumber(L, 13, 0.0f);

        Matrix4 matrix(x, y, a, sx, sy, ox, oy, kx, ky);
        luax_catchexcept(L, [&]() { index = self->addf(newText, wrap, align, matrix); });
    }

    lua_pushinteger(L, index + 1);

    return 1;
}

int Wrap_TextBatch::clear(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    luax_catchexcept(L, [&]() { self->clear(); });

    return 0;
}

int Wrap_TextBatch::setFont(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);
    auto* font = luax_checktype<FontBase>(L, 2);

    luax_catchexcept(L, [&]() { self->setFont(font); });

    return 0;
}

int Wrap_TextBatch::getFont(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    auto* font = self->getFont();
    luax_pushtype(L, font);

    return 1;
}

int Wrap_TextBatch::getWidth(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    int index = luaL_optinteger(L, 2, 0) - 1;
    lua_pushnumber(L, self->getWidth(index));

    return 1;
}

int Wrap_TextBatch::getHeight(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    int index = luaL_optinteger(L, 2, 0) - 1;
    lua_pushnumber(L, self->getHeight(index));

    return 1;
}

int Wrap_TextBatch::getDimensions(lua_State* L)
{
    auto* self = luax_checktextbatch(L, 1);

    int index = luaL_optinteger(L, 2, 0) - 1;
    lua_pushnumber(L, self->getWidth(index));
    lua_pushnumber(L, self->getHeight(index));

    return 2;
}

// clang-format off
static constexpr luaL_Reg functions[] =
{
    { "set",            Wrap_TextBatch::set           },
    { "setf",           Wrap_TextBatch::setf          },
    { "add",            Wrap_TextBatch::add           },
    { "addf",           Wrap_TextBatch::addf          },
    { "clear",          Wrap_TextBatch::clear         },
    { "setFont",        Wrap_TextBatch::setFont       },
    { "getFont",        Wrap_TextBatch::getFont       },
    { "getWidth",       Wrap_TextBatch::getWidth      },
    { "getHeight",      Wrap_TextBatch::getHeight     },
    { "getDimensions",  Wrap_TextBatch::getDimensions }
};
// clang-format on

namespace love
{
    TextBatch* luax_checktextbatch(lua_State* L, int index)
    {
        return luax_checktype<TextBatch>(L, index);
    }

    int open_textbatch(lua_State* L)
    {
        return luax_register_type(L, &TextBatch::type, functions);
    }
} // namespace love
