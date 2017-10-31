#pragma once

namespace love
{
	class m_Image
	{
		static love::m_Image * instance;

		public:
			static love::m_Image * Instance() 
			{
				if (!instance)
					instance = new love::m_Image();
				return instance;
			}

			static int NewImageData(lua_State * L);
	};
}