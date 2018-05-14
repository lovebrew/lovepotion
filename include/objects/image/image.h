#pragma once

class Image
{
	public:
		Image() {};
		Image(const char * path);
		~Image();

		int GetWidth();
		int GetHeight();

		C2D_Image GetImage();
		Tex3DS_SubTexture * GetSubTex();
		void SetQuad(Tex3DS_SubTexture * subtex);

	private:
		int width;
		int height;

		C2D_SpriteSheet sheet;
		C2D_Image image;

		Tex3DS_SubTexture subtex;
};