#pragma once

class Image
{
	public:
		Image() {};
		Image(const char * path);
		~Image();

		u16 GetWidth();
		u16 GetHeight();

		vector<u8> GetImage();

	private:
		u32 width;
		u32 height;

		vector<u8> sheet;
};
