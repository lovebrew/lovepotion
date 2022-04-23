#include "objects/videostream/videostream.h"

using namespace love;

love::Type VideoStream::type("VideoStream", &Stream::type);

void VideoStream::SetSync(love::FrameSync* sync)
{
    this->frameSync = frameSync;
}

love::FrameSync* VideoStream::GetSync() const
{
    return this->frameSync;
}

void VideoStream::Play()
{
    this->frameSync->Play();
}

void VideoStream::Pause()
{
    this->frameSync->Pause();
}

void VideoStream::Seek(double offset)
{
    this->frameSync->Seek(offset);
}

double VideoStream::Tell() const
{
    return this->frameSync->Tell();
}

bool VideoStream::IsPlaying() const
{
    return this->frameSync->IsPlaying();
}
