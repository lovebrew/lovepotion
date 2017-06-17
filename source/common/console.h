#pragma once

namespace love
{
	class Console 
	{
		public:
			Console();
			bool IsEnabled();
			void Enable(gfxScreen_t screen);
			
			void ThrowError(lua_State * L);
			void ThrowError(const char * message);
			gfxScreen_t GetScreen();
			
		private:
			bool hasPrinted;
			gfxScreen_t screen;
	};
} //namespace love