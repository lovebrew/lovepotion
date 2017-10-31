#pragma once

namespace love
{	
	class Font
	{
		public:
			const char * DefaultInit();
			const char * Init(const char * path);
			
			int GetWidth(char glyph);
			int GetHeight();

			love::Glyph * GetGlyph(char glyph);
			love::Image * GetSheet();

			void AddChars();
			void Collect();

		private:
			love::File * configFile;
			love::Image * bitmap;
			nlohmann::json configJson;
			int chars;
			love::Glyph ** glyphs;
	};
}