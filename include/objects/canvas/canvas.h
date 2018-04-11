class Canvas
{
	public:
		Canvas(int width, int height);
		Canvas() {};
		~Canvas();

		SDL_Texture * GetTexture();
		int GetWidth();
		int GetHeight();

	private:
		SDL_Texture * texture;
		int width;
		int height;
};