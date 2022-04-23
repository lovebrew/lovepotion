#include "objects/videostream/sync/deltasync.h"
#include "modules/thread/types/lock.h"

using namespace love;
using thread::Lock;

DeltaSync::DeltaSync() : playing(false), position(0), speed(1)
{}

DeltaSync::~DeltaSync()
{}

double DeltaSync::GetPosition() const
{
    return this->position;
}

void DeltaSync::Update(double dt)
{
    Lock lock(this->mutex);

    if (this->playing)
        this->position += this->speed * dt;
}

void DeltaSync::Play()
{
    this->playing = true;
}

void DeltaSync::Pause()
{
    this->playing = false;
}

void DeltaSync::Seek(double time)
{
    Lock lock(this->mutex);
    this->position = time;
}

bool DeltaSync::IsPlaying() const
{
    return this->playing;
}
