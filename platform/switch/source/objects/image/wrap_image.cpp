#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/image/image.h"
#include "objects/image/wrap_image.h"

#include "objects/imagedata/imagedata.h"

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGE
#define CLASS_NAME  "Image"

int imageNew(lua_State * L)
{
    bool memory = false;

    string path;
    size_t size = 0;
    ImageData * imageData = NULL;

    if (lua_type(L, 1) == LUA_TSTRING)
        path = luaL_checkstring(L, 1);
    else if (lua_type(L, 1) == LUA_TUSERDATA)
        imageData = (ImageData *)luaobj_checkudata(L, 1, LUAOBJ_TYPE_IMAGEDATA);

    if ((path.substr(0, 7) == "nogame:") || (path.substr(0, 6) == "error:"))
        memory = true;

    const char * data = path.c_str();

    if (!memory && size == 0) //not imagedata
        LOVE_VALIDATE_FILE_EXISTS(data);

    void * raw_self = luaobj_newudata(L, sizeof(Image));

    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    if (imageData == NULL)
        new (raw_self) Image(data, memory);
    else
    {
        /*u32 pixels = imageData->GetData();
        int size = imageData->GetSize();

        new (raw_self) Image(&pixels, size);*/
    }

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