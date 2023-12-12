#pragma once

#include <common/console.hpp>
#include <common/object.hpp>
#include <common/resource.hpp>

#include <utilities/driver/renderer/vertex.hpp>

#include <stddef.h>

#include <string>
#include <vector>

namespace love
{
    template<Console::Platform T>
    class Graphics;

    class Buffer : public Object, public Resource
    {
        static Type type;
        static constexpr size_t SHADER_STORAGE_BUFFER_MAX_STRIDE = 0x800;

        enum MapType
        {
            MAP_WRITE_INVALIDATE,
            MAP_READ_ONLY
        };

        struct DataDeclaration
        {
            std::string name;
            vertex::DataFormat format;
            int arrayLength;

            DataDeclaration(const std::string& name, vertex::DataFormat format,
                            int arrayLength = 0) :
                name(name),
                format(format),
                arrayLength(arrayLength)
            {}
        };

        struct DataMember
        {
            DataDeclaration declaration;
            vertex::DataFormatInfo info;
            size_t offset;
            size_t size;

            DataMember(const DataDeclaration& declaration) :
                declaration(declaration),
                info(vertex::GetDataFormatInfo(declaration.format)),
                offset(0),
                size(0)
            {}
        };

        struct Settings
        {
            vertex::BufferUsageFlags usageFlags;
            vertex::BufferDataUsage dataUsage;
            bool zeroInitialize;

            Settings(uint32_t usageFlags, vertex::BufferDataUsage dataUsage) :
                usageFlags((vertex::BufferUsageFlags)usageFlags),
                dataUsage(dataUsage),
                zeroInitialize(false)
            {}
        };

        Buffer(Graphics<Console::ALL>* gfx, const Settings& settings,
               const std::vector<DataDeclaration>& format, size_t size, size_t arrayLength);

        virtual ~Buffer();
    };
} // namespace love
