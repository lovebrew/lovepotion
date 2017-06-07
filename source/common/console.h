#pragma once

namespace love
{
	class Console 
	{
		public:
			Console();
			bool IsEnabled();
			void Enable();
			
			void ThrowError(lua_State * L);
			void ThrowError(const char * message);
		
		private:
			bool hasPrinted;
	};
} //namespace love