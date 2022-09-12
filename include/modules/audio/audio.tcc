#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

namespace love
{
    template<Console::Platform T = Console::ALL>
    class Audio : public Module
    {
        ~Audio();

        ModuleType GetModuleType() const override
        {
            return M_AUDIO;
        }

        const char* GetModuleName() const override
        {
            return "love.audio";
        }

        // Source* NewSource(Decoder* decoder) ;

        // Source* NewSource(SoundData* soundData) ;

        // Source* NewSource(int sampleRate, int bitDepth, int channels, int buffers) ;

        int GetActiveSourceCount();

        int GetMaxSources();

        // bool Play(Source* source) ;

        // void Stop(Source* source) ;

        // void Stop(const std::vector<Source*>& sources) ;

        void Stop();

        // void Pause(Source* source) ;

        // void Pause(const std::vector<Source*>& sources) ;

        // std::vector<Source*> Pause() ;
    };
} // namespace love
