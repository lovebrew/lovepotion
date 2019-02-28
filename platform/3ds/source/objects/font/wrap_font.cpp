#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/font/font.h"
#include "objects/font/wrap_font.h"

#define CLASS_TYPE LUAOBJ_TYPE_FONT
#define CLASS_NAME "Font"

int fontNew(lua_State * L)
{
    string path = "";
    int index = 1;

    float size = 15;

    if (!lua_isnoneornil(L, 1) && lua_type(L, 1) == LUA_TSTRING)
    {
        path = luaL_checkstring(L, 1);
        index += 1;
    }

    if (!lua_isnoneornil(L, index) && lua_type(L, index) == LUA_TNUMBER)
    {
        size = luaL_checknumber(L, index);
        index += 1;
    }

    void * raw_self = luaobj_newudata(L, sizeof(Font));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    Font * self;
    
    if (path != "")
        self = new (raw_self) Font(path, size);
    else
        self = new (raw_self) Font();

    love_register(L, index, self);

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

    char * data = self->ToString();

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
        { "__gc",       fontGC        },
        { "__tostring", fontToString  },
        { "getHeight",  fontGetHeight },
        { "getWidth",   fontGetWidth  },
        { "new",        fontNew       },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, fontNew, reg);

    return 1;
}
