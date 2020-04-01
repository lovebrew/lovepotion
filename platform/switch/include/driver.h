#pragma once

namespace AudioDriver
{
    inline Mutex drvMutex = 0;
    AudioDriver driver;

    void Update(void *);
}
