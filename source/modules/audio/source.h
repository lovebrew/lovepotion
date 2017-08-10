#pragma once

extern bool channelList[24];

namespace love
{
	class Source
	{
		public:
			const char * Init(const char * path, const char * type);
			
			void Play();
			void Stop();
			void SetLooping(bool loop);
			void SetVolume(float volume);
			int IsPlaying();
			float GetDuration();
			float Tell();

			long FillBuffer(void * audio);
			const char * Decode();
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
			u32 bitrate;

			bool loop;

			int streamOffset;
			int chunkSamples;
			int currentSection;

			float mix[12];
			ndspInterpType interp;
			
			bool stream; //if it should stream
			bool fillBuffer;
	};
}

extern std::vector<love::Source *> streams;