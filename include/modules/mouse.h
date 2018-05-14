/*
** mouse.h
** @brief : Mouse coordinates (touch), and if pressing
*/

namespace Mouse
{
	//Löve2D Functions

	int GetX(lua_State * L);
	int GetY(lua_State * L);
	int GetPosition(lua_State * L);

	int IsDown(lua_State * L);

	//End Löve2D Functions

	int Register(lua_State * L);
}