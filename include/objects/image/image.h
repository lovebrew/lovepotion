#pragma once

class Image
{
	public:
		Image() {};
		Image(const char * path);
		~Image();

		int GetWidth();
		int GetHeight();

		u8 * GetImage();

	private:
		int width;
		int height;

		u8 * sheet;
};