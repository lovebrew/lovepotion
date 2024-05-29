#include "modules/graphics/wrap_Font.hpp"

#include <algorithm>

namespace love
{
    FontBase* luax_checkfont(lua_State* L, int index)
    {
        return luax_checktype<FontBase>(L, index);
    }

    void luax_checkcoloredstring(lua_State* L, int index, std::vector<ColoredString>& strings)
    {
        ColoredString coloredString {};
        coloredString.color = Color::WHITE;

        if (lua_istable(L, index))
        {
            const int length = luax_objlen(L, index);

            for (int i = 0; i <= length; i++)
            {
                lua_rawgeti(L, index, i + 1);

                if (lua_istable(L, -1))
                {
                    for (int j = 1; j <= 4; j++)
                        lua_rawgeti(L, -j, j);

                    coloredString.color.r = (float)luaL_checknumber(L, -4);
                    coloredString.color.g = (float)luaL_checknumber(L, -3);
                    coloredString.color.b = (float)luaL_checknumber(L, -2);
                    coloredString.color.a = (float)luaL_optnumber(L, -1, 1.0f);

                    lua_pop(L, 4);
                }
                else
                {
                    coloredString.string = luaL_checkstring(L, -1);
                    strings.push_back(coloredString);
                }
            }
        }
        else
        {
            coloredString.string = luaL_checkstring(L, index);
            strings.push_back(coloredString);
        }
    }
} // namespace love
