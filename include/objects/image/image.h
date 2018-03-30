#pragma once

class Image
{
	public:
		Image() {};
		Image(const char * path);
		~Image();

		u16 GetWidth();
		u16 GetHeight();

		SDL_Texture * GetImage();

	private:
		u32 width;
		u32 height;
		SDL_Texture * texture;
};
