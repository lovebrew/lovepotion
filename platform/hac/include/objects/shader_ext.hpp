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
            uint32_t magic;
            uint32_t header_sz;
            uint32_t control_sz;
            uint32_t code_sz;
            uint32_t programs_off;
            uint32_t num_programs;
        };

        static constexpr size_t HEADER_SIZE = sizeof(DkshHeader);

        Shader();

        Shader(Data* vertex, Data* fragment);

        virtual ~Shader();

        void Attach();

        std::optional<DekoStage> Vertex()
        {
            return this->program.vertex;
        }

        std::optional<DekoStage> Fragment()
        {
            return this->program.fragment;
        }

        static void AttachDefault(StandardShader type);

        bool Validate(const char* filepath, DekoStage& stage, std::string& error) const;

        bool Validate(Data* data, DekoStage& stage, std::string& error) const;

        void LoadDefaults(StandardShader type);

      private:
        Program program;
    };

} // namespace love
