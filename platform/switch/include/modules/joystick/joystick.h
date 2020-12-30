#pragma once

#include "modules/joystick/joystickc.h"
#include "modules/thread/types/threadable.h"

#include "modules/joystick/joypool.h"

namespace love
{
    class Joystick : public common::Joystick
    {
        public:
            Joystick();

            virtual ~Joystick();

            bool AddVibration(Gamepad * gamepad, size_t id);

            static std::atomic<bool> threadFinished;
            static thread::Mutex mutex;

        private:
            class PoolThread : public Threadable
            {
                public:
                    PoolThread(JoystickPool * pool);

                    virtual ~PoolThread();

                    void SetFinish();

                    void ThreadFunction();

                protected:
                    JoystickPool * pool;
                    std::atomic<bool> finish;
            };

            std::vector<Gamepad *> vibrations;

            JoystickPool * pool;
            PoolThread * poolThread;
    };
}