#pragma once

namespace love
{
	class Image
	{
		public:
			const char * Init(const char * path, bool memory, bool VRAM);
			void BasicInit(bool VRAM);
			void BasicInit(int width, int height);

			void Decode(unsigned char * buffer, unsigned textureWidth, unsigned textureHeight);
			void LoadTexture(void * data, int width, int height);
			void DecodeFile();
			void DecodeMemory(const unsigned char * in, size_t size);
			void SetFilter(const char * min, const char * mag);

			C3D_Tex * GetTexture();
			int GetWidth();
			int GetHeight();
			int NextPow2(unsigned int x);
		private:
			C3D_Tex * texture;
			const char * path;

			int width;
			int height;
			
			bool isPremultiplied;
			bool VRAM;

			GPU_TEXTURE_FILTER_PARAM minFilter;
			GPU_TEXTURE_FILTER_PARAM magFilter;
			
			
	};
}