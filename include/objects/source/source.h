#pragma once

class Source
{
	public:
		Source() {};
		Source(const char * path, bool stream);
		~Source();

		void Play();
		void SetLooping(bool loop);
		bool IsPlaying();
		void Stop();
		void Reset();
		void Update();
		bool IsStatic();
		void SetVolume(float volume);

		int GetAudioChannel();
		const char * GetPath();

	private:
		bool IsValid();

		Mix_Chunk * sound;
		Mix_Music * music;

		int channel;
		bool loop;
		bool stream;
};