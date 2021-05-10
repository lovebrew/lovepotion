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

        bool Initialize() override;

        ~Debugger();

      private:
        Debugger();
    };
} // namespace love
