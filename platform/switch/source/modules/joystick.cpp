#include "modules/joystick/joystick.h"

#include "modules/timer/timer.h"

using namespace love;

Joystick::PoolThread::PoolThread(JoystickPool * pool) : pool(pool),
                                                        finish(false)
{
    this->threadName = "VibrationPool";
}

Joystick::PoolThread::~PoolThread()
{}

void Joystick::PoolThread::ThreadFunction()
{
    while (true)
    {
        if (this->finish)
            return;

        this->pool->Update();
        svcSleepThread(5000000);
    }
}

void Joystick::PoolThread::SetFinish()
{
    this->finish = true;
}

Joystick::Joystick() : pool(nullptr),
                       poolThread(nullptr)
{
    try
    {
        this->pool = new JoystickPool();
    }
    catch (love::Exception &)
    {
        throw;
    }

    this->poolThread = new PoolThread(pool);
    this->poolThread->Start();
}

Joystick::~Joystick()
{
    this->poolThread->SetFinish();
    this->poolThread->Wait();

    delete this->poolThread;
    delete this->pool;
}

bool Joystick::AddVibration(Gamepad * gamepad, size_t id)
{
    return this->pool->AssignGamepad(gamepad, id);
}