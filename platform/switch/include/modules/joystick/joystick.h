#pragma once

#include "modules/joystick/joystickc.h"
#include "modules/thread/types/threadable.h"

#include "pools/vibration.h"

namespace love
{
    class Joystick : public common::Joystick
    {
      public:
        Joystick();

        virtual ~Joystick();

        bool AddVibration(Gamepad* gamepad, size_t id);

      protected:
        size_t GetActiveControllerCount();

      private:
        class VibrationThread : public Threadable
        {
          public:
            VibrationThread(VibrationPool* pool);

            virtual ~VibrationThread();

            void SetFinish();

            void ThreadFunction();

          protected:
            VibrationPool* pool;
            std::atomic<bool> finish;
        };

        std::vector<Gamepad*> vibrations;

        VibrationPool* pool;
        VibrationThread* poolThread;
    };
} // namespace love
