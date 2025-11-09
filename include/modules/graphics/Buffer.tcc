#pragma once

#include "common/Object.hpp"
#include "common/Optional.hpp"

#include "modules/graphics/Resource.hpp"
#include "modules/graphics/vertex.hpp"

#include <stddef.h>
#include <string>
#include <vector>

namespace love
{
    class GraphicsBase;

    class BufferBase : public Object, public Resource
    {
      public:
        static Type type;

        static int bufferCount;
        static int64_t totalGraphicsMemory;

        static constexpr size_t SHADER_STORAGE_BUFFER_MAX_SRIDE = 2048;

        enum MapType
        {
            MAP_WRITE_INVALIDATE,
            MAP_READ_ONLY
        };

        struct DataDeclaration
        {
            std::string name;
            DataFormat format;
            int arrayLength;
            int bindingLocation;

            DataDeclaration(const std::string& name, DataFormat format, int length = 0, int location = -1) :
                name(name),
                format(format),
                arrayLength(length),
                bindingLocation(location)
            {}
        };

        using BufferFormat = std::vector<DataDeclaration>;

        struct DataMember
        {
            DataDeclaration declaration;
            DataFormatInfo info;
            size_t offset;
            size_t size;

            DataMember(const DataDeclaration& other) :
                declaration(other),
                info(getDataFormatInfo(declaration.format)),
                offset(0),
                size(0)
            {}
        };

        struct Settings
        {
            BufferUsageFlags usage;
            BufferDataUsage dataUsage;
            bool zeroInitialize;
            std::string debugName;

            Settings(uint32_t usage, BufferDataUsage dataUsage) :
                usage((BufferUsageFlags)usage),
                dataUsage(dataUsage),
                zeroInitialize(false),
                debugName()
            {}
        };

        BufferBase(GraphicsBase* graphics, const Settings& settings, const BufferFormat& bufferFormat,
                   size_t size, size_t length);

        virtual ~BufferBase();

        size_t getSize() const
        {
            return this->size;
        }

        BufferUsageFlags getUsageFlags() const
        {
            return this->usage;
        }

        BufferDataUsage getDataUsage() const
        {
            return this->dataUsage;
        }

        bool isMapped() const
        {
            return this->mapped;
        }

        size_t getArrayLength() const
        {
            return this->arrayLength;
        }

        size_t getArrayStride() const
        {
            return this->arrayStride;
        }

        const std::vector<DataMember>& getDataMembers() const
        {
            return this->members;
        }

        const DataMember& getDataMember(int index) const
        {
            return this->members[index];
        }

        size_t getMemberOffset(int index) const
        {
            return this->members[index].offset;
        }

        int getDataMemberIndex(const std::string& name) const;

        int getDataMemberIndex(int bindingLocation) const;

        const std::string& getDebugName() const
        {
            return this->debugName;
        }

        void setImmutable(bool immutable)
        {
            this->immutable = immutable;
        }

        bool isImmutable() const
        {
            return this->immutable;
        }

        virtual void* map(MapType map, size_t offset, size_t size) = 0;

        virtual void unmap(size_t usedOffset, size_t usedSize) = 0;

        virtual bool fill(size_t offset, size_t size, const void* data) = 0;

        void clear(size_t offset, size_t size);

        virtual void copyTo(BufferBase* dest, size_t sourceOffset, size_t destOffset, size_t size) = 0;

        virtual ptrdiff_t getTexelBufferHandle() const = 0;

        static std::vector<DataDeclaration> getCommonFormatDeclaration(CommonFormat format);

        class Mapper
        {
          public:
            Mapper(BufferBase& buffer, MapType type = MAP_WRITE_INVALIDATE) : buffer(buffer)
            {
                this->data = buffer.map(type, 0, buffer.getSize());
            }

            ~Mapper()
            {
                this->buffer.unmap(0, this->buffer.getSize());
            }

            BufferBase& buffer;
            void* data;
        };

      protected:
        virtual void clearInternal(size_t offset, size_t size) = 0;

        std::vector<DataMember> members;
        size_t arrayLength;
        size_t arrayStride;

        size_t size;

        BufferUsageFlags usage;
        BufferDataUsage dataUsage;

        std::string debugName;

        bool mapped;
        MapType mappedType;
        bool immutable;
    };
} // namespace love
