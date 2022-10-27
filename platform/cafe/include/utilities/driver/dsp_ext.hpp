#pragma once

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <utilities/driver/dsp.tcc>

#include <SDL2/SDL_mixer.h>

extern "C"
{
    /* todo - remove when wut supports these */
    void AXSetMasterVolume(uint32_t volume);
    int16_t AXGetMasterVolume();
}

namespace love
{
    template<>
    class DSP<Console::CAFE> : public DSP<Console::ALL>
    {
      public:
        static DSP& Instance()
        {
            static DSP instance;
            return instance;
        }

        DSP();

        ~DSP();

        void Initialize();

        void Update()
        {}

        void SetMasterVolume(float volume);

        float GetMasterVolume() const;

        bool ChannelReset(size_t id);

        void ChannelSetVolume(size_t id, float volume);

        float ChannelGetVolume(size_t id);

        size_t ChannelGetSampleOffset(size_t id);

        bool ChannelAddBuffer(size_t id, Mix_Chunk* buffer, bool looping);

        void ChannelPause(size_t id, bool paused = true);

        bool IsChannelPaused(size_t id);

        bool IsChannelPlaying(size_t id);

        void ChannelStop(size_t id);

      private:
        static constexpr int MIX_INIT_ALL = MIX_INIT_MP3 | MIX_INIT_OGG;
    };
} // namespace love
