#include "common/runtime.h"

#include "common/drawable.h"

#include "objects/font/font.h"
#include "objects/text/text.h"
#include "objects/text/wrap_text.h"

#define CLASS_TYPE LUAOBJ_TYPE_TEXT
#define CLASS_NAME "Text"

int textNew(lua_State * L)
{
    Font * font = (Font *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_FONT);
    const char * text = luaL_checkstring(L, 2);

    void * raw_self = luaobj_newudata(L, sizeof(Text));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    new (raw_self) Text(font, text);

    return 1;
}

int textGetWidth(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushnumber(L, self->GetWidth());

    return 1;
}

int textGetHeight(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushnumber(L, self->GetHeight());

    return 1;
}

int textGetDimensions(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushnumber(L, self->GetWidth());
    lua_pushnumber(L, self->GetHeight());

    return 2;
}

int textClear(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->Clear();

    return 0;
}

int textSet(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    const char * text = luaL_checkstring(L, 2);
    self->Set(text, 1280, "left");

    return 0;
}

int textSetF(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    const char * text = luaL_checkstring(L, 1);
    int limit = luaL_checknumber(L, 2);
    string align = luaL_optstring(L, 3, "left");

    self->Set(text, limit, align);

    return 0;
}

int textGetFont(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    Font * font = self->GetFont();
    love_push_userdata(L, font);

    return 1;
}

int textSetFont(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    Font * font = (Font *)luaobj_checkudata(L, 2, LUAOBJ_TYPE_FONT);
    self->SetFont(font);

    return 0;
}

int textGC(lua_State * L)
{
    return 0;
}

int textToString(lua_State * L)
{
    Text * self = (Text *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initTextClass(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "__gc",          textGC            },
        { "__tostring",    textToString      },
        { "clear",         textClear         },
        { "set",           textSet           },
        { "setf",          textSetF          },
        { "getDimensions", textGetDimensions },
        { "getFont",       textGetFont       },
        { "getHeight",     textGetHeight     },
        { "getWidth",      textGetWidth      },
        { "new",           textNew           },
        { "setFont",       textSetFont       },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, textNew, reg);

    return 1;
}