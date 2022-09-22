#include <modules/timer_ext.hpp>
#include <utilities/pool/poolthread.hpp>

using namespace love;

#define Timer() (Module::GetInstance<Timer<Console::Which>>(Module::M_TIMER))

PoolThread::PoolThread(VibrationPool* pool) : vibrations(pool), finish(false)
{
    this->name = "VibrationPool";
}

PoolThread::~PoolThread()
{}

void PoolThread::ThreadFunction()
{
    while (!this->finish)
    {
        if (this->vibrations)
            this->vibrations->Update();

        Timer()->Sleep(0.0005);
    }
}

void PoolThread::SetFinish()
{
    this->finish = true;
}
