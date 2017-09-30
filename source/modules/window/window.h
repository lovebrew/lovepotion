#pragma once

namespace love
{
	class Window
	{
		static love::Window * instance;

		public:
			static love::Window * Instance()
			{
				if (!instance)
					instance = new love::Window();
				return instance;
			}
			static int SetMode(lua_State * L);

		private:
			Window();
	};
}