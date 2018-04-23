#pragma once

class Quad
{
	public:
		Quad() {};
		Quad(float subTextureX, float subTextureY, float subTextureWidth, float subTextureHeight, float atlasWidth, float atlasHeight);

		int GetX();
		int GetY();
		int GetWidth();
		int GetHeight();

		void SetViewport(int x, int y, int width, int height);

	private:
		int NextPow2(unsigned int x);

		float x;
		float y;

		float width;
		float height;
};