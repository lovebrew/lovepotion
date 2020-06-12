#pragma once

namespace love
{
    class LOVE_Thread;

    class Threadable : public love::Object
    {
        public:
            static love::Type type;

            Threadable();
            ~Threadable();

            virtual void ThreadFunction() = 0;

            bool Start();

            void Wait();

            bool IsRunning() const;

            const char * GetThreadName() const;

        private:
            friend class LOVE_Thread;

        protected:
            LOVE_Thread * owner;
            std::string threadName;
    };
}