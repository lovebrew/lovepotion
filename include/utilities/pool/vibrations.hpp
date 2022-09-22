#pragma once

#include <utilities/haptics/vibration.tcc>
#include <utilities/threads/threads.hpp>

#include <map>
#include <queue>
#include <vector>

using Vibration = love::Vibration<love::Console::Which>;

namespace love
{
    class VibrationPool
    {
      public:
        VibrationPool();

        ~VibrationPool();

        void Update();

        bool AddVibration(::Vibration* vibration);

      private:
        std::map<::Vibration*, size_t> vibrating;
        std::queue<size_t> available;
        love::mutex mutex;

        bool ReleaseVibration(::Vibration* vibration);

        bool FindVibration(::Vibration* vibration, size_t& id);
    };
} // namespace love
