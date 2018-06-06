#include "common/runtime.h"

#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

#define CLASS_TYPE LUAOBJ_TYPE_FONT
#define CLASS_NAME "Font"

int fontNew(lua_State * L)
{
    const char * path = NULL;
    int size = 14;

    /*
    ** Either first arg is string
    ** Or a number
    */
    if (!lua_isnoneornil(L, 1))
    {
        if (lua_type(L, 1) == LUA_TSTRING)
        {
            path = luaL_checkstring(L, 1);
            size = luaL_checknumber(L, 2);
        }
        else if (lua_type(L, 1) == LUA_TNUMBER)
            size = luaL_checknumber(L, 1);
    }

    void * raw_self = luaobj_newudata(L, sizeof(Font));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    if (path != NULL)
        Font * self = new (raw_self) Font(path, size);
    else
        Font * self = new (raw_self) Font(size);

    return 1;
}

//Font:getWidth
int fontGetWidth(lua_State * L)
{
    Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);
    const char * text = luaL_checkstring(L, 2);

    lua_pushnumber(L, self->GetWidth(text));

    return 1;
}

//Font:getHeight
int fontGetHeight(lua_State * L)
{
    Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushinteger(L, self->GetSize());

    return 1;
}

int fontToString(lua_State * L)
{
    Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString(CLASS_NAME);

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int fontGC(lua_State * L)
{
    Font * self = (Font *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->~Font();

    return 0;
}

int initFontClass(lua_State * L) 
{
    luaL_Reg reg[] = 
    {
        {"new",            fontNew            },
        {"getWidth",    fontGetWidth    },
        {"getHeight",    fontGetHeight    },
        {"__gc",        fontGC            },
        {"__tostring",    fontToString    },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

    return 1;
}