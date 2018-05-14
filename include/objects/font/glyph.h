class Glyph
{
	public:
		Glyph() {};
		Glyph(nlohmann::json values, int sheetWidth, int sheetHeight);

		Quad * GetQuad();

		int GetXOffset();
		int GetYOffset();
		int GetXAdvance();

	private:
		Quad quad;

		int xoffset;
		int xadvance;
		int yoffset;
};