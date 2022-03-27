#include "modules/video/video.h"
#include "modules/video/worker.h"

using namespace love;

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
    TheoraStream* stream = new TheoraStream(file);
    this->workerThread->AddStream(stream);

    return stream;
}
