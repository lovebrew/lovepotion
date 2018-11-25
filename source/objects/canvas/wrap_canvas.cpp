#include "common/runtime.h"

#include "common/drawable.h"
#include "objects/canvas/canvas.h"
#include "objects/canvas/wrap_canvas.h"

#include "modules/window.h"

#define CLASS_TYPE LUAOBJ_TYPE_CANVAS
#define CLASS_NAME "Canvas"

int canvasNew(lua_State * L)
{
    int width = luaL_optnumber(L, 1, 1280);
    int height = luaL_optnumber(L, 2, 720);

    void * raw_self = (void *)luaobj_newudata(L, sizeof(Canvas));
    
    luaobj_setclass(L, CLASS_TYPE, CLASS_NAME);

    new (raw_self) Canvas(width, height);

    return 1;
}

int canvasGetWidth(lua_State * L)
{
    Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);
    
    lua_pushnumber(L, self->GetWidth());
    
    return 1;
}

int canvasGetHeight(lua_State * L)
{
    Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);
    
    lua_pushnumber(L, self->GetHeight());
    
    return 1;
}

int canvasGetDimensions(lua_State * L)
{
    Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);

    lua_pushnumber(L, self->GetWidth());
    lua_pushnumber(L, self->GetHeight());

    return 2;
}

int canvasRenderTo(lua_State * L)
{
    Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);

    if (lua_isfunction(L, 2))
    {
    
        self->SetAsTarget();

        int function_index = luaL_ref(L, 2);

        lua_rawgeti(L, LUA_REGISTRYINDEX, function_index);
        lua_pcall(L, 0, 0, 0);

        luaL_unref(L, 2, function_index);

        SDL_SetRenderTarget(Window::GetRenderer(), NULL);
    }
    else
        return luaL_error(L, "Function expected, got %s", lua_tostring(L, 2));

    return 0;
}

int canvasGC(lua_State * L)
{
    Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);

    self->~Canvas();

    return 0;
}

int canvasToString(lua_State * L)
{
    Canvas * self = (Canvas *)luaobj_checkudata(L, 1, CLASS_TYPE);

    char * data = self->ToString();

    lua_pushstring(L, data);

    free(data);

    return 1;
}

int initCanvasClass(lua_State * L) 
{
    luaL_Reg reg[] = 
    {
        { "__gc",          canvasGC            },
        { "__tostring",    canvasToString      },
        { "getDimensions", canvasGetDimensions },
        { "getWidth",      canvasGetWidth      },
        { "getHeight",     canvasGetHeight     },
        { "new",           canvasNew           },
        { "renderTo",      canvasRenderTo      },
        { 0, 0 }
    };

    luaobj_newclass(L, CLASS_NAME, NULL, canvasNew, reg);

    return 1;
}