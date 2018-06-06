#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGE
#define CLASS_NAME  "Image"

int imageNew(lua_State * L)
{
    bool memory = false;
    lua_getglobal(L, "in_error");
    if (lua_type(L, -1) == LUA_TBOOLEAN && lua_toboolean(L, -1) == 1)
        memory = true;
    
    lua_setglobal(L, "in_error");
    
    const char * path = luaL_checkstring(L, 1);

    if (!memory)
        LOVE_VALIDATE_FILE_EXISTS(path);

    void * raw_self = luaobj_newudata(L, sizeof(Image));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    Image * self = new (raw_self) Image(path, memory);

    return 1;
}

//Image:getWidth
int imageGetWidth(lua_State * L)
{
    Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushnumber(L, self->GetWidth());

    return 1;
}

//Image:getHeight
int imageGetHeight(lua_State * L)
{
    Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushnumber(L, self->GetHeight());

    return 1;
}

//Image:getDimensions
int imageGetDimensions(lua_State * L)
{
    Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushnumber(L, self->GetWidth());
    lua_pushnumber(L, self->GetHeight());

    return 2;
}

int imageToString(lua_State * L)
{
    Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString(CLASS_NAME);

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int imageGC(lua_State * L)
{
    Image * self = (Image *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->~Drawable();

    return 0;
}

int initImageClass(lua_State * L) 
{
    luaL_Reg reg[] = {
        {"new",                imageNew            },
        {"getWidth",        imageGetWidth        },
        {"getHeight",        imageGetHeight        },
        {"getDimensions",    imageGetDimensions    },
        //{"setFilter",        imageSetFilter        },
        {"__gc",            imageGC                },
        {"__tostring",        imageToString        },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, imageNew, reg);

    return 1;
}