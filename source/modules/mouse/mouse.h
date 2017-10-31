#pragma once

namespace love
{
	class Mouse
	{
		static love::Mouse * instance;

		public:
			static love::Mouse * Instance()
			{
				if (!instance)
					instance = new love::Mouse();
				return instance;
			}

			static int IsDown(lua_State * L);
			static int GetX(lua_State * L);
			static int GetY(lua_State * L);
		
		private:
			Mouse();

	};
}