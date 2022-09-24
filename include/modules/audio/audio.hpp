#pragma once

#include <common/console.hpp>
#include <common/module.hpp>

#include <objects/source_ext.hpp>

#include <utilities/pool/poolthread.hpp>
#include <utilities/pool/sources.hpp>

namespace love
{
    class Audio : public Module
    {
      public:
        Audio();

        ~Audio();

        ModuleType GetModuleType() const override
        {
            return M_AUDIO;
        }

        const char* GetName() const override
        {
            return "love.audio";
        }

        Source<Console::Which>* NewSource(Decoder* decoder) const;

        Source<Console::Which>* NewSource(SoundData* soundData) const;

        Source<Console::Which>* NewSource(int sampleRate, int bitDepth, int channels,
                                          int buffers) const;

        int GetActiveSourceCount() const;

        int GetMaxSources() const;

        bool Play(Source<Console::Which>* source);

        void Stop(Source<Console::Which>* source);

        // void Stop(const std::vector<Source*>& sources) ;

        // void Stop();

        // void Pause(Source* source) ;

        // void Pause(const std::vector<Source*>& sources) ;

        // std::vector<Source*> Pause() ;

      private:
        AudioPool* pool;
        PoolThread* thread;
    };
} // namespace love
