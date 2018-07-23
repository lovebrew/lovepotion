/*
** modules/graphics.h
** @brief : Used for rendering graphics (primitives, images, text)
*/

#pragma once

namespace Graphics
{
    void Initialize();
    bool IsInitialized();

	float GetXScale();
	float GetYScale();
	
    //Löve2D Functions

    int GetWidth(lua_State * L);
    int GetHeight(lua_State * L);

    int Draw(lua_State * L);

    int Clear(lua_State * L);
    int Present(lua_State * L);

    int SetColor(lua_State * L);
    
    int SetBackgroundColor(lua_State * L);
    int GetBackgroundColor(lua_State * L);

    int Push(lua_State * L);
    int Pop(lua_State * L);
    int Translate(lua_State * L);
    int Scale(lua_State * L);
    int Rotate(lua_State * L);
    int Shear(lua_State * L);
    int Origin(lua_State * L);

    int SetCanvas(lua_State * L);

    int Rectangle(lua_State * L);
    int Polygon(lua_State * L);
    int Line(lua_State * L);
    int Circle(lua_State * L);
    int Arc(lua_State * L);
    int Points(lua_State * L);

    int SetScissor(lua_State * L);

    int GetRendererInfo(lua_State * L);
    int SetDefaultFilter(lua_State * L);

    int GetFont(lua_State * L);
    int SetFont(lua_State * L);
    int SetNewFont(lua_State * L);
    int Print(lua_State * L);

    //End Löve2D Functions

    int Register(lua_State * L);

    void Exit();
}