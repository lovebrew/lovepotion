#pragma once

namespace love
{
	class Audio
	{
		static love::Audio * instance;

		public:
			static love::Audio * Instance()
			{
				if (!instance)
					instance = new love::Audio();
				return instance;
			}
		
		static int Play(lua_State * L);
		static int Stop(lua_State * L);
		static int SetVolume(lua_State * L);

	};
}