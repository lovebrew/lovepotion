#pragma once

enum Acquire
{
    RETAIN,
    NORETAIN
};

namespace love
{
    template <typename T>
    class StrongReference
    {
        public:
            StrongReference() : object(nullptr) {}

            StrongReference(T * object, Acquire acquire = Acquire::RETAIN) : object(object)
            {
                if (object && acquire == Acquire::RETAIN)
                    object->Retain();
            }

            StrongReference(const StrongReference & other) : object(other.Get())
            {
                if (object)
                    object->Retain();
            }

            StrongReference(StrongReference && other) : object(other.object)
            {
                other.object = nullptr;
            }

            ~StrongReference()
            {
                if (object)
                    object->Release();
            }

            StrongReference & operator = (const StrongReference & other)
            {
                Set(other.Get());
                return *this;
            }

            T * operator ->() const
            {
                return object;
            }

            explicit operator bool() const
            {
                return object != nullptr;
            }

            operator T * () const
            {
                return object;
            }

            void Set(T * obj, Acquire acquire = Acquire::RETAIN)
            {
                if (obj && acquire == Acquire::RETAIN)
                    obj->Retain();

                if (object)
                    object->Release();

                object = obj;
            }

            T * Get() const
            {
                return object;
            }

        private:
            T * object;
    };
}
