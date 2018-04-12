class Drawable
{
	public:
		void Draw(SDL_Rect * quad, double x, double y, double rotation, double scalarX, double scalarY);
		~Drawable();

		int GetWidth();
		int GetHeight();

	protected:
		int width;
		int height;

		SDL_Texture * texture;
};