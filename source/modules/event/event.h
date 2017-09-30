#pragma once

namespace love
{
	class Event
	{
		static love::Event * instance;

		public:
			love::Event * Instance()
			{
				if (!instance)
					instance = new love::Event();
				return instance;
			}
			static int Quit(lua_State * L);

		private:
			Event();
	};
}