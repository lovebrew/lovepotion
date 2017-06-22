#pragma once

namespace love
{
	class Glyph
	{
		public:
			Glyph(const char * glyph, nlohmann::json config);
			love::Quad * GetQuad();
			int GetOffsetX();
			char GetChar();

		private:
			love::Quad * quad;
			
			//These are NOT the same as quad width!
			//xadvance is how far the pen moves, 
			//offset is where it actually starts drawing relative to the pen
			char glyph;
			int width;
			int height;
	};
}