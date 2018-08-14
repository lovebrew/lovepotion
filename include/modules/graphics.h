/*
** modules/graphics.h
** @brief : Used for rendering graphics (primitives, images, text)
*/

#pragma once

namespace Graphics
{
    void Initialize();
    bool IsInitialized();

    //Löve2D Functions
    int Clear(lua_State * L);
    int Present(lua_State * L);

    int Draw(lua_State * L);
    int Print(lua_State * L);

    int SetBackgroundColor(lua_State * L);
    int SetColor(lua_State * L);

    int GetWidth(lua_State * L);
    int GetHeight(lua_State * L);

    int GetRendererInfo(lua_State * L);

    int SetScreen(lua_State * L);
    int SetFont(lua_State * L);

    //End Löve2D Functions
    C3D_RenderTarget * GetScreen(gfxScreen_t screen, gfx3dSide_t side);
    void Clear(gfxScreen_t screen, gfx3dSide_t side);
    void DrawOn(gfxScreen_t screen, gfx3dSide_t side);


    u32 ConvertColor(Color & color);

    int Register(lua_State * L);

    void Exit();
}