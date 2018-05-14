/*
** keyboard.h
** @brief : Used for handling input
*/

namespace Keyboard
{
	void Initialize();

	//Löve2D Functions

	int IsDown(lua_State * L);
	int SetTextInput(lua_State * L);

	//End Löve2D Functions

	int Scan(lua_State * L);

	int Register(lua_State * L);
}