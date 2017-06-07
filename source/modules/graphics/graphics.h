#pragma once

namespace love
{
	class Graphics
	{
		static love::Graphics * instance;

		public:
			~Graphics();
			static love::Graphics * Instance()
			{
				if (!instance)
					instance = new love::Graphics();
				return instance;
			}

			int NewImage(lua_State * L);
			static int GetWidth(lua_State * L);
			static int GetHeight(lua_State * L);
			static int SetScreen(lua_State * L);
			static int Present(lua_State * L);
			

		private:
			Graphics();
			static gfxScreen_t currentScreen;

	};
}