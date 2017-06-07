#pragma once

namespace love
{
	class System
	{
		static love::System * instance;

		public:
			static love::System * Instance() 
			{
				if (!instance)
					instance = new love::System();
				return instance;
			}

			static int GetOS(lua_State * L);
			static int GetProcessorCount(lua_State * L);
			static int GetPowerInfo(lua_State * L);

			static int GetRegion(lua_State * L);
			static int GetModel(lua_State * L);
			static int GetWifiStrength(lua_State * L);
			static int GetWifiStatus(lua_State * L);
			static int GetSystemLanguage(lua_State * L);

		private:
			System();
	};
}