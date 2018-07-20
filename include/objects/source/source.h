#pragma once

class Source : public Object
{
    public:
        Source() {};
        Source(const char * path, const string & type);
        ~Source();

        void Play();
        void SetLooping(bool loop);
        bool IsLooping();
        bool IsPlaying();
        void Stop();
        void Pause();
        void Resume();
        bool IsStatic();
        void SetVolume(float volume);
    private:
        bool IsValid();

        Mix_Chunk * sound;
        Mix_Music * music;

        int channel;
        bool loop;
        bool stream;
};

extern std::map<int, Source *> streams;