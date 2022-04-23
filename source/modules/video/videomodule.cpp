#include "modules/video/videomodule.h"
#include "modules/video/worker.h"

using namespace love;

#include "debug/logger.h"

VideoModule::VideoModule()
{
    this->workerThread = new Worker();
    this->workerThread->Start();
}

VideoModule::~VideoModule()
{
    delete this->workerThread;
}

VideoStream* VideoModule::NewVideoStream(File* file)
{
    TheoraStream* stream = new TheoraStream(file);
    this->workerThread->AddStream(stream);

    return stream;
}
