class Drawable : public Object
{
	public:
		Drawable();
		void Draw(SDL_Rect * quad, double x, double y, double rotation, double scalarX, double scalarY, SDL_Color color);
		~Drawable();

		int GetWidth();
		int GetHeight();

	protected:
		int width;
		int height;

		SDL_Texture * texture;
		SDL_Surface * surface;
		SDL_Surface * changedSurface;

		double horScale;
};