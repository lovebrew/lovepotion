#include "modules/video/worker.h"

#include "modules/thread/types/lock.h"
#include "objects/thread/thread.h"

#include "common/delay.h"
#include "modules/timer/timer.h"

using namespace love;

Worker::Worker() : stopping(false)
{
    this->threadName = "VideoWorker";
}

Worker::~Worker()
{}

void Worker::AddStream(TheoraStream* stream)
{
    thread::Lock lock(this->mutex);

    this->streams.push_back(stream);
    this->condition->Broadcast();
}

void Worker::Stop()
{
    this->stopping = true;

    {
        thread::Lock lock(this->mutex);
        this->condition->Broadcast();
    }

    this->owner->Wait();
}

void Worker::ThreadFunction()
{
    double lastFrame = Timer::GetTime();

    while (true)
    {
        love::Sleep(2);

        thread::Lock lock(this->mutex);

        while (!this->stopping && this->streams.empty())
        {
            this->condition->Wait(this->mutex);
            lastFrame = Timer::GetTime();
        }

        if (this->stopping)
            return;

        double currentFrame = Timer::GetTime();
        double delta        = currentFrame - lastFrame;

        lastFrame = currentFrame;

        for (auto it = this->streams.begin(); it != this->streams.end(); ++it)
        {
            TheoraStream* stream = *it;

            if (stream->GetReferenceCount() == 1)
            {
                this->streams.erase(it);
                break;
            }

            stream->ThreadedFillBackBuffer(delta);
        }
    }
}
