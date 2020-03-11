#include "common/runtime.h"
#include "objects/font/wrap_font.h"

using namespace love;

love::Type love::Font::type("Font", &Object::type);

int Wrap_Font::GetWidth(lua_State * L)
{
    love::Font * self = Wrap_Font::CheckFont(L, 1);
    const char * text = luaL_checkstring(L, 2);

    lua_pushnumber(L, self->GetWidth(text));

    return 1;
}

int Wrap_Font::GetHeight(lua_State * L)
{
    love::Font * self = Wrap_Font::CheckFont(L, 1);

    float height = self->GetHeight();

    lua_pushnumber(L, height);

    return 1;
}

Font * Wrap_Font::CheckFont(lua_State * L, int index)
{
    return Luax::CheckType<Font>(L, index);
}

void Wrap_Font::CheckColoredString(lua_State * L, int index, std::vector<Font::ColoredString> & strings)
{
    Font::ColoredString coloredString;
    coloredString.color = {1, 1, 1, 1};

    if (lua_istable(L, index))
    {
        int length = (int)lua_objlen(L, index);

        for (int i = 1; i <= length; i++)
        {
            lua_rawgeti(L, index, i);

            if (lua_istable(L, -1))
            {
                for (int j = 1; j <= 4; j++)
                    lua_rawgeti(L, -j, j);

                coloredString.color.r = luaL_checknumber(L, -4);
                coloredString.color.g = luaL_checknumber(L, -3);
                coloredString.color.b = luaL_checknumber(L, -2);
                coloredString.color.a = luaL_optnumber(L, -1, 1);

                lua_pop(L, 4);
            }
            else
            {
                coloredString.string = luaL_checkstring(L, -1);
                strings.push_back(coloredString);
            }

            lua_pop(L, 1);
        }
    }
    else
    {
        coloredString.string = luaL_checkstring(L, index);
        strings.push_back(coloredString);
    }

}

int Wrap_Font::Register(lua_State * L)
{
    luaL_Reg reg[] = {
        { "getHeight",     GetHeight },
        { "getWidth",      GetWidth  },
        { 0, 0 }
    };

    return Luax::RegisterType(L, &love::Font::type, reg, nullptr);
}
