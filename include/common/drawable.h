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

    protected:
        void Flip(double x, double y, double scalarX, double scalarY);

        int width;
        int height;

        SDL_Texture * texture;

        Viewport viewport;

        SDL_RendererFlip flip;
};