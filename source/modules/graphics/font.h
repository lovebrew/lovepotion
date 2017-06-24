#pragma once

namespace love
{	
	class Font
	{
		public:
			char * Init(const char * path);
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