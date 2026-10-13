#include "modules/video/VideoStream.hpp"
#include "modules/audio/Source.hpp"
#include "modules/video/Video.hpp"
#include <mutex>

namespace love
{
    Type VideoStream::type("VideoStream", &Object::type);

    void VideoStream::setSync(FrameSync* frame)
    {
        this->sync = frame;
    }

    VideoStream::FrameSync* VideoStream::getSync() const
    {
        return this->sync;
    }

    void VideoStream::play()
    {
        this->sync->play();
    }

    void VideoStream::pause()
    {
        this->sync->pause();
    }

    void VideoStream::seek(double offset)
    {
        this->sync->seek(offset);
    }

    double VideoStream::tell() const
    {
        return this->sync->tell();
    }

    bool VideoStream::isPlaying() const
    {
        return this->sync->isPlaying();
    }

    VideoStream::Frame::Frame() : yplane(nullptr), cbplane(nullptr), crplane(nullptr)
    {}

    VideoStream::Frame::~Frame()
    {
        delete[] this->yplane;
        delete[] this->cbplane;
        delete[] this->crplane;
    }

    void VideoStream::FrameSync::copyState(const FrameSync* other)
    {
        this->seek(other->tell());
        if (other->isPlaying())
            this->play();
        else
            this->pause();
    }

    double VideoStream::FrameSync::tell() const
    {
        return this->getPosition();
    }

    VideoStream::DeltaSync::DeltaSync() : playing(false), position(0), speed(1)
    {}

    VideoStream::DeltaSync::~DeltaSync()
    {}

    double VideoStream::DeltaSync::getPosition() const
    {
        return this->position;
    }

    void VideoStream::DeltaSync::update(double dt)
    {
        std::unique_lock lock(this->mutex);
        if (this->playing)
            this->position += dt * this->speed;
    }

    void VideoStream::DeltaSync::play()
    {
        this->playing = true;
    }

    void VideoStream::DeltaSync::pause()
    {
        this->playing = false;
    }

    void VideoStream::DeltaSync::seek(double time)
    {
        std::unique_lock lock(this->mutex);
        this->position = time;
    }

    bool VideoStream::DeltaSync::isPlaying() const
    {
        return this->playing;
    }

    VideoStream::SourceSync::SourceSync(SourceBase* source) : source(source)
    {}

    double VideoStream::SourceSync::getPosition() const
    {
        return this->source->tell(SourceBase::UNIT_SECONDS);
    }

    void VideoStream::SourceSync::play()
    {
        this->source->play();
    }

    void VideoStream::SourceSync::pause()
    {
        this->source->pause();
    }

    void VideoStream::SourceSync::seek(double time)
    {
        this->source->seek(time, SourceBase::UNIT_SECONDS);
    }

    bool VideoStream::SourceSync::isPlaying() const
    {
        return this->source->isPlaying();
    }
} // namespace love
