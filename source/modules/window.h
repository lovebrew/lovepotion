#pragma once

namespace love
{
	class Window
	{
		static love::Window * instance;

		public:
			~Window();
			static love::Window * Instance()
			{
				if (!instance)
					instance = new love::Window();
				return instance;
			}
		private:
			Window();
	};
}