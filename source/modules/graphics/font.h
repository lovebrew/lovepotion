#pragma once

namespace love
{	
	class Font
	{
		public:
			char * DefaultInit();
			char * Init(const char * path);
			
			int GetWidth(char * glyph);
			int GetHeight();

			love::Glyph * GetGlyph(char glyph);
			love::Image * GetSheet();

		private:
			love::File * configFile;
			love::Image * bitmap;
			nlohmann::json configJson;
			std::vector<love::Glyph *> glyphs;
			int chars;
	};
}