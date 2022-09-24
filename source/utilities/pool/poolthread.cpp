#include <modules/timer_ext.hpp>
#include <utilities/pool/poolthread.hpp>

#include <utilities/driver/dsp_ext.hpp>

using namespace love;

#define Timer() (Module::GetInstance<Timer<Console::Which>>(Module::M_TIMER))

PoolThread::PoolThread(VibrationPool* pool) : vibrations(pool), sources(nullptr), finish(false)
{
    this->name = "VibrationPool";
}

PoolThread::PoolThread(AudioPool* pool) : vibrations(nullptr), sources(pool), finish(false)
{
    this->name = "AudioPool";
}

PoolThread::~PoolThread()
{}

void PoolThread::ThreadFunction()
{
    while (!this->finish)
    {
        if (this->vibrations)
        {
            this->vibrations->Update();
            Timer()->Sleep(0.0005);
        }

        if (this->sources)
        {
            this->sources->Update();
            DSP<Console::Which>::Instance().Update();
        }
    }
}

void PoolThread::SetFinish()
{
    this->finish = true;
}
