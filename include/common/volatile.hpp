#pragma once

#include <list>

namespace love
{
    class Volatile
    {
      private:
        static std::list<Volatile*> all;

      public:
        Volatile();

        virtual ~Volatile();

        virtual bool LoadVolatile() = 0;

        virtual void UnloadVolatile() = 0;

        static bool LoadAll();

        static void UnloadAll();
    };
} // namespace love
