#pragma once

namespace love
{
	class Glyph
	{
		public:
			Glyph(nlohmann::json config);
			love::Quad * GetQuad();
			int GetXAdvance();
			int GetYOffset();
			int GetXOffset();
			int GetWidth();

			int GetChar();
			Glyph() {};

		private:
			love::Quad * quad;
			
			//These are NOT the same as quad width!
			//xadvance is how far the pen moves, 
			//offset is where it actually starts drawing relative to the pen
			int glyph;
			int xadvance;
			int yoffset;
			int xoffset;
			int width;
	};
}