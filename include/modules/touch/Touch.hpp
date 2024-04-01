#pragma once

#include "common/Module.hpp"
#include "events.hpp"

#include <vector>

namespace love
{
    class Touch : public Module
    {
      public:
        Touch();

        virtual ~Touch()
        {}

        const std::vector<Finger>& getTouches();

        const Finger& getTouch(int64_t id);

        void onEvent(SubEventType type, const Finger& info);

      private:
        std::vector<Finger> touches;
    };
} // namespace love
