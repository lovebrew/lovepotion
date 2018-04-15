class Canvas : public Drawable
{
	public:
		Canvas(int width, int height);
		Canvas() {};

		int GetWidth();
		int GetHeight();

		void SetAsTarget();
};