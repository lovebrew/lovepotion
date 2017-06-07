#pragma once

extern bool channelList[24];

namespace love
{
	class Source
	{
		public:
			char * Init(const char * path, const char * type);
			void Play();
			void FillBuffer(void * audio, size_t offset, size_t size);
			void DecodeWave();
			void DecodeOgg();
			int GetOpenChannel();
			void Update();

			char * GetAudio();
			u32 GetSamples();
			
		private:
			ndspWaveBuf waveBuffer[2];
			char * data;
			FILE * file;
			//OggVorbis_File vorbisFile;
	
			u32 rate;
			u32 channels;
			u32 encoding;
			u32 nsamples;
			u32 size;
			bool loop;
			int audiochannel;

			int samplesPerBuffer;

			size_t streamOffset = 0;

			float mix[12];
			ndspInterpType interp;
			
			bool stream; //if it should stream
			bool fillBuffer = false;
	};
}

extern love::Source * streams[24];