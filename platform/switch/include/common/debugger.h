#pragma once

#include "common/debug/debuggerc.h"

namespace love
{
    class Debugger : public common::Debugger
    {
      public:
        static Debugger& Instance()
        {
            static Debugger instance;
            return instance;
        }

        ~Debugger();

      private:
        int sockfd;

        Debugger();
    };
} // namespace love