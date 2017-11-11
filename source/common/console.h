#pragma once

namespace love
{
	class Console 
	{
		public:
			Console();
			bool IsEnabled();
			void Enable(gfxScreen_t screen);

			int ThrowError(lua_State * L);
			int ThrowError(const char * format, ...);
			gfxScreen_t GetScreen();
			char * GetError();

		private:
			char * errorMessage;
			gfxScreen_t screen;
			bool enabled;
	};
} //namespace love