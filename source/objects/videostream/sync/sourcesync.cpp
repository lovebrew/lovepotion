#include "objects/videostream/sync/sourcesync.h"

using namespace love;

SourceSync::SourceSync(Source* source) : source(source)
{}

double SourceSync::GetPosition() const
{
    return this->source->Tell(Source::UNIT_SECONDS);
}

void SourceSync::Play()
{
    this->source->Play();
}

void SourceSync::Pause()
{
    this->source->Pause();
}

void SourceSync::Seek(double time)
{
    this->source->Seek(time, Source::UNIT_SECONDS);
}

bool SourceSync::IsPlaying() const
{
    return this->source->IsPlaying();
}
