#include "modules/joystick/joystick.h"

#include "modules/timer/timer.h"

using namespace love;

Joystick::VibrationThread::VibrationThread(VibrationPool * pool) : pool(pool),
                                                                   finish(false)
{
    this->threadName = "VibrationPool";
}

Joystick::VibrationThread::~VibrationThread()
{}

void Joystick::VibrationThread::ThreadFunction()
{
    while (!this->finish)
    {
        this->pool->Update();
        svcSleepThread(5000000);
    }
}

void Joystick::VibrationThread::SetFinish()
{
    this->finish = true;
}

Joystick::Joystick() : pool(nullptr),
                       poolThread(nullptr)
{

    this->pool = new VibrationPool();

    this->poolThread = new VibrationThread(pool);
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