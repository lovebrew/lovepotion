#include "modules/video/video.h"
#include "modules/video/worker.h"

using namespace love;

#include "debug/logger.h"

Video::Video()
{
    this->workerThread = new Worker();
    this->workerThread->Start();
}

Video::~Video()
{
    delete this->workerThread;
}

VideoStream* Video::NewVideoStream(File* file)
{
    LOG("making a stream");
    TheoraStream* stream = new TheoraStream(file);
    LOG("adding a stream to thread %x", this->workerThread);
    this->workerThread->AddStream(stream);
    LOG("we exploded, probs %x", this->workerThread);

    return stream;
}
