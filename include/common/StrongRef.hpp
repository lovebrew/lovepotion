#pragma once

#include "common/Object.hpp"

namespace love
{
    template<typename T>
    class StrongRef
    {
      public:
        StrongRef() : object(nullptr)
        {}

        StrongRef(T* obj, Acquire acquire = Acquire::RETAIN) : object(obj)
        {
            if (this->object && acquire == Acquire::RETAIN)
                this->object->retain();
        }

        StrongRef(const StrongRef& other) : object(other.get())
        {
            if (this->object)
                this->object->retain();
        }

        StrongRef(StrongRef&& other) : object(other.object)
        {
            other.object = nullptr;
        }

        ~StrongRef()
        {
            if (this->object)
                this->object->release();
        }

        StrongRef& operator=(const StrongRef& other)
        {
            this->set(other.get());
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

        void set(T* obj, Acquire acquire = Acquire::RETAIN)
        {
            if (obj && acquire == Acquire::RETAIN)
                obj->retain();

            if (this->object)
                this->object->release();

            this->object = obj;
        }

        T* get() const
        {
            return this->object;
        }

      private:
        T* object;
    };
} // namespace love
