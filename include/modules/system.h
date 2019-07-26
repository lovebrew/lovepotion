/*
** modules/system.h
** @brief : Used to get system information
*/

class System
{
    public:
        static void Initialize();

        static void Exit();

        static int Register(lua_State * L);

    private:
        //Löve2D Functions

        static int GetOS(lua_State * L);

        static int GetProcessorCount(lua_State * L);
        
        static int GetPowerInfo(lua_State * L);

        static int GetRegion(lua_State * L);
        
        static int GetUsername(lua_State * L);
        
        static int GetLanguage(lua_State * L);

        static int GetInternetStatus(lua_State * L);

        //End Löve2D Functions
};
