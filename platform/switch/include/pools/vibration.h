#pragma once

#include "objects/gamepad/gamepad.h"

#include "thread/types/lock.h"
#include "thread/types/mutex.h"

#include <map>

namespace love
{
    class VibrationPool
    {
      public:
        ~VibrationPool()
        {}

        bool IsRunning();

        bool FindGamepad(Gamepad* gamepad);

        bool AssignGamepad(Gamepad* gamepad, size_t id);

        bool ReleaseGamepad(Gamepad* gamepad, bool stop = true);

        void Finish();

        void Update();

        thread::Lock Lock();

      private:
        std::atomic<bool> running = true;
        std::map<Gamepad*, size_t> vibrating;
        thread::MutexRef mutex;
    };
} // namespace love