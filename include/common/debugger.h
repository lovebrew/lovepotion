#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

namespace love
{
    class Debugger
    {
        public:
            static Debugger & Instance() {
                static Debugger instance;
                return instance;
            }

            const bool IsInited() {
                return initialized;
            }

            const int GetSockfd() {
                return this->sockfd;
            }

            ~Debugger();

        private:
            int sockfd;

            bool initialized;

            Debugger();
    };
}