#pragma once

namespace love
{
	class Image
	{
		public:
			char * Init(const char * path);

		private:
			C3D_Tex * texture;
			int NextPow2(unsigned int x);
	};
}