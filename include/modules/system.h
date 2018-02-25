/*
** modules/system.h
** @brief : Used to get system information
*/

namespace System
{
	void Initialize();
	void Exit();

	//Löve2D Functions

	int GetOS(lua_State * L);
	int GetProcessorCount(lua_State * L);
	int GetPowerInfo(lua_State * L);

	int GetRegion(lua_State * L);
	int GetUsername(lua_State * L);
	int GetLanguage(lua_State * L);

	int HasWifiConnection(lua_State * L);
	int GetWifiStrength(lua_State * L);

	//End Löve2D Functions

	int Register(lua_State * L);
}