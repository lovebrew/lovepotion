#include "modules/video/videomodule.h"
#include "modules/video/worker.h"

using namespace love;

VideoModule::VideoModule()
{
    this->workerThread = new Worker();
    this->workerThread->Start();
}

VideoModule::~VideoModule()
{
    this->workerThread->Stop();
    delete this->workerThread;
}

VideoStream* VideoModule::NewVideoStream(File* file)
{
    TheoraStream* stream = new TheoraStream(file);
    this->workerThread->AddStream(stream);

    return stream;
}
