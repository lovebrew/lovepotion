#include "common/delay.hpp"

#include "modules/timer/Timer.hpp"
#include "modules/video/theora/Video.hpp"

namespace love
{
    Worker::Worker() : stopping(false)
    {
        this->threadName = "VideoWorker";
    }

    Worker::~Worker()
    {
        this->stop();
    }

    void Worker::addStream(TheoraVideoStream* stream)
    {
        std::unique_lock lock(this->mutex);
        this->streams.push_back(stream);
        this->condition.notify_all();
    }

    void Worker::stop()
    {
        {
            std::unique_lock lock(this->mutex);
            this->stopping = true;
            this->condition.notify_all();
        }
        this->owner->wait();
    }

    void Worker::run()
    {
        double lastFrame = Timer::getTime();

        while (true)
        {
            sleep(2);

            std::unique_lock lock(this->mutex);
            while (!this->stopping && this->streams.empty())
            {
                this->condition.wait(this->mutex);
                lastFrame = Timer::getTime();
            }

            if (this->stopping)
                return;

            double currentFrame = Timer::getTime();
            double dt           = currentFrame - lastFrame;
            lastFrame           = currentFrame;

            for (auto it = this->streams.begin(); it != this->streams.end(); ++it)
            {
                TheoraVideoStream* stream = *it;
                if (stream->getReferenceCount() == 1)
                {
                    this->streams.erase(it);
                    break;
                }
                stream->threadedFillBackBuffer(dt);
            }
        }
    }

    VideoModule::VideoModule() : VideoModuleBase("love.video.theora")
    {
        this->worker = new Worker();
        this->worker->start();
    }

    VideoModule::~VideoModule()
    {
        delete this->worker;
    }

    VideoStream* VideoModule::newVideoStream(FileBase* file)
    {
        auto* stream = new TheoraVideoStream(file);
        this->worker->addStream(stream);
        return stream;
    }
} // namespace love
