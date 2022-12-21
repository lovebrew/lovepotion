#pragma once

#include <objects/shader/shader.tcc>

#include <utilities/deko3d/CMemPool.h>

#include <deko3d.hpp>

namespace love
{
    template<>
    class Shader<Console::HAC> : public Shader<Console::ALL>
    {
      public:
        struct DekoStage
        {
            dk::Shader shader;
            CMemPool::Handle codeMemory;
        };

        struct Program
        {
            std::optional<DekoStage> vertex;
            std::optional<DekoStage> fragment;
        };

        struct DkshHeader
        {
            uint32_t magic;     // DKSH_MAGIC
            uint32_t header_sz; // sizeof(DkshHeader)
            uint32_t control_sz;
            uint32_t code_sz;
            uint32_t programs_off;
            uint32_t num_programs;
        };

        Shader();

        Shader(Data* vertex, Data* fragment);

        virtual ~Shader();

        void Attach();

        const dk::Shader* const Vertex() const
        {
            return &this->program.vertex.value().shader;
        }

        const dk::Shader* const Fragment() const
        {
            return &this->program.fragment.value().shader;
        }

        static void AttachDefault(StandardShader type);

        bool Validate(const char* filepath, DekoStage& stage) const;

        bool Validate(Data* data, DekoStage& stage) const;

        void LoadDefaults(StandardShader type);

      private:
        Program program;
    };

} // namespace love
