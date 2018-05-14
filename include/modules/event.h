/*
** event.h
** @brief	: Handles events.. like quitting.. and other things
*/

namespace Event
{
	//Löve2D Functions

	int Quit(lua_State * L);

	//End Löve2D Functions

	int Register(lua_State * L);
}