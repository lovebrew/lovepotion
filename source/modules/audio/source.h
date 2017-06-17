#pragma once

extern bool channelList[24];

namespace love
{
	class Source
	{
		public:
			char * Init(const char * path, const char * type);
			void Play();
			long FillBuffer(void * audio, bool first);
			char * Decode();
			int GetOpenChannel();
			void Update();
			void Reset();
			
			char * data;
			int audiochannel;

		private:
			ndspWaveBuf waveBuffer[2];

			FILE * fileHandle;
			OggVorbis_File vorbisFile;

			u32 rate;
			u32 channels;
			u32 encoding;
			u32 nsamples;
			u32 size;
			bool loop;

			int streamOffset;
			int chunkSamples;
			int currentSection;

			float mix[12];
			ndspInterpType interp;
			
			bool stream; //if it should stream
			bool fillBuffer = false;
	};
}

extern love::Source * streams[24];