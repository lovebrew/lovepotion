#pragma once

namespace love
{
    class Object
    {
        public:
            Object(const Object & other);
            Object();

            virtual ~Object() = 0;

            void Retain();
            void Release();

            int GetReferenceCount() const;

            static love::Type type;

        private:
            std::atomic<int> count;
    };
}
