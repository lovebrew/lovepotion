#pragma once

namespace love
{
	class Keyboard
	{
		static love::Keyboard * instance;

		public:
			static love::Keyboard * Instance()
			{
				if (!instance)
					instance = new love::Keyboard();
				return instance;
			}

			static int IsDown(lua_State * L);
			static int HasTextInput(lua_State * L);
			static int SetTextInput(lua_State * L);


		private:
			Keyboard();
			static bool TEXT_INPUT;
			static love::SoftwareKeyboard * keyboard;
	};
}