class Drawable : public Object
{
    public:
        Drawable(char * type);
        Drawable() {};
        void Draw(const Viewport & viewport, double x, double y, double rotation, double scalarX, double scalarY, SDL_Color color);
        ~Drawable();

        int GetWidth();
        int GetHeight();

        Viewport GetViewport();

    private:
        SDL_Surface * Flip(SDL_Rect & quad, SDL_Rect & position, double rotation, double x, double y);

    protected:
        int width;
        int height;

        SDL_Texture * texture;
        SDL_Surface * surface;

        Viewport viewport;

        double horScale;
};