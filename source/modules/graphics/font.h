#pragma once

namespace love
{
	class Font
	{
		public:
			char * Init(const char * path);

		private:
			FILE * fileHandle;

	};
}