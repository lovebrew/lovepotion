#pragma once

namespace love
{
	class ImageData
	{
		public:
			const char * Init(const char * path);
			int GetWidth();
			int GetHeight();

			void SetPixel(int x, int y, u32 color);
			pixel GetPixel(int x, int y);

		private:
			const char * Decode();

			int width;
			int height;
			unsigned char * buffer;
			const char * path;
			std::string data;
	};
}