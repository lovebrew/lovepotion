#pragma once

enum Acquire
{
    RETAIN,
    NORETAIN
};

namespace love
{
    template<typename T>
    class StrongReference
    {
      public:
        StrongReference() : object(nullptr)
        {}

        StrongReference(T* obj, Acquire acquire = Acquire::RETAIN) : object(obj)
        {
            if (this->object && acquire == Acquire::RETAIN)
                this->object->Retain();
        }

        StrongReference(const StrongReference& other) : object(other.Get())
        {
            if (this->object)
                this->object->Retain();
        }

        StrongReference(StrongReference&& other) : object(other.object)
        {
            other.object = nullptr;
        }

        ~StrongReference()
        {
            if (this->object)
                this->object->Release();
        }

        StrongReference& operator=(const StrongReference& other)
        {
            this->Set(other.Get());
            return *this;
        }

        T* operator->() const
        {
            return this->object;
        }

        explicit operator bool() const
        {
            return this->object != nullptr;
        }

        operator T*() const
        {
            return this->object;
        }

        void Set(T* obj, Acquire acquire = Acquire::RETAIN)
        {
            if (obj && acquire == Acquire::RETAIN)
                obj->Retain();

            if (this->object)
                this->object->Release();

            this->object = obj;
        }

        T* Get() const
        {
            return this->object;
        }

      private:
        T* object;
    };
} // namespace love
