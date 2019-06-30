#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGE
#define CLASS_NAME  "Image"

int imageNew(lua_State * L)
{
    bool memory = false;

    string path = luaL_checkstring(L, 1);

    if (path.find("nogame:", 0, 7) != string::npos || path.find("error:", 0, 6) != string::npos)
        memory = true;

    // if (!memory)
    // {
    //     size_t length = path.length();
    //     path = path.replace(length - 4, 4, ".t3x");

    //     if (!LOVE_VALIDATE_FILE_EXISTS_CLEAN(path.c_str()))
    //         return luaL_error(L, "Could not open image %s. Does not exist.", path.c_str());
    // }

    void * raw_self = luaobj_newudata(L, sizeof(Image));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    new (raw_self) Image(path.c_str(), memory);

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

    char * data = self->ToString();

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
        { "__gc",          imageGC            },
        { "__tostring",    imageToString      },
        { "getDimensions", imageGetDimensions },
        { "getHeight",     imageGetHeight     },
        { "getWidth",      imageGetWidth      },
        { "new",           imageNew           },
        //{ "setFilter",     imageSetFilter     },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, imageNew, reg);

    return 1;
}