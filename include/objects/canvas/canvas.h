class Canvas : public Drawable
{
	public:
		Canvas(int width, int height);
		Canvas() {};
		~Canvas();

		int GetWidth();
		int GetHeight();

		void SetAsTarget();
};