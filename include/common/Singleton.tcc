#pragma once

namespace love
{
    template<class T>
    class Singleton
    {
      public:
        static T& getInstance()
        {
            static T instance;
            return instance;
        }
    };
} // namespace love
