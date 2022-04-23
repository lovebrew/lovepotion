#include "objects/videostream/sync/framesync.h"

using namespace love;

void FrameSync::CopyState(const FrameSync* other)
{
    this->Seek(other->Tell());

    if (other->IsPlaying())
        this->Play();
    else
        this->Pause();
}

double FrameSync::Tell() const
{
    return this->GetPosition();
}
