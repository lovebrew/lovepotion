#include "common/runtime.h"

#include "objects/imagedata/wrap_imagedata.h"
#include "objects/imagedata/imagedata.h"

#define CLASS_TYPE  LUAOBJ_TYPE_IMAGEDATA
#define CLASS_NAME  "ImageData"

int imageDataNew(lua_State * L)
{
    int width = 0;
    int height = 0;
    string format;

    
    if (lua_gettop(L) == 1)
    {
        const char * filename = luaL_checkstring(L, 1);
        
        void * raw_self = luaobj_newudata(L, sizeof(ImageData));

        luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);
 
        new (raw_self) ImageData(filename);
    }
    else
    {
        int width = luaL_checkinteger(L, 1);
        int height = luaL_checkinteger(L, 2);
    
        format = luaL_optstring(L, 3, "rgba8");
    }

    return 1;
}

int imageDataGetPixel(lua_State * L)
{
    ImageData * self = (ImageData *)luaobj_checkudata(L, 1, CLASS_TYPE);

    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    SDL_Color color = self->GetPixel(x, y);

    lua_pushnumber(L, color.r);
    lua_pushnumber(L, color.g);
    lua_pushnumber(L, color.b);
    lua_pushnumber(L, color.a);

    return 4;
}

int initImageDataClass(lua_State * L)
{
    luaL_Reg reg[] = {
        //{ "__gc",          imageGC            },
        //{ "__tostring",    imageToString      },
        //{ "getDimensions", imageGetDimensions },
        //{ "getHeight",     imageGetHeight     },
        //{ "getWidth",      imageGetWidth      },
        { "getPixel",      imageDataGetPixel      },
        { "new",           imageDataNew           },
        //{ "setFilter",     imageSetFilter     },
        { 0, 0 },
    };

    luaobj_newclass(L, CLASS_NAME, NULL, imageDataNew, reg);

    return 1;
}