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
			int ThrowError(const char * message);
			gfxScreen_t GetScreen();
			const char * GetError();

		private:
			const char * errorMessage;
			gfxScreen_t screen;
			bool enabled;
	};
} //namespace love