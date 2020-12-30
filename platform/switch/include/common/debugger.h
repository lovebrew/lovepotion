#pragma once

#include "common/debug/debuggerc.h"

namespace love
{
    class Debugger : public common::Debugger
    {
        public:
            ~Debugger();

        private:
            int sockfd;

            Debugger();
    };
}