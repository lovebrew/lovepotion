#include "common/runtime.h"

#include "modules/mod_image.h"
#include "objects/imagedata/wrap_imagedata.h"
#include "objects/imagedata/imagedata.h"

int ImageModule::Register(lua_State * L)
{
    luaL_Reg reg[] = 
    {
        { "newImageData", imageDataNew },
        { 0, 0 },
    };

    luaL_newlib(L, reg);
    
    return 1;
}