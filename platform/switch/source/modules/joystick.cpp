#include "modules/joystick/joystick.h"

#include "modules/timer/timer.h"

using namespace love;

static constexpr size_t MAX_GAMEPADS = 4;

Joystick::VibrationThread::VibrationThread(VibrationPool* pool) : pool(pool), finish(false)
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

Joystick::Joystick() : pool(nullptr), poolThread(nullptr)
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

size_t Joystick::GetActiveControllerCount()
{
    size_t active = 0;

    for (size_t index = 0; index < MAX_GAMEPADS; index++)
    {
        HidNpadIdType id  = static_cast<HidNpadIdType>(HidNpadIdType_No1 + index);
        uint32_t styleSet = hidGetNpadStyleSet(id);

        /* check for handheld */
        if (styleSet == 0)
            styleSet = hidGetNpadStyleSet(HidNpadIdType_Handheld);

        if (styleSet != 0)
            active++;
    }

    return active;
}

bool Joystick::AddVibration(Gamepad* gamepad, size_t id)
{
    return this->pool->AssignGamepad(gamepad, id);
}
