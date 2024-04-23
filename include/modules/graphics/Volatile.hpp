#pragma once

#include <list>

namespace love
{
    class Volatile
    {
      public:
        Volatile();

        virtual ~Volatile();

        virtual bool loadVolatile() = 0;

        virtual void unloadVolatile() = 0;

        static bool loadAll();

        static void unloadAll();

      private:
        static std::list<Volatile*> all;
    };
} // namespace love
