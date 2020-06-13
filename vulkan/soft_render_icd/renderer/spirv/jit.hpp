#pragma once

#include "../../../../vulkan-headers/include/vulkan/vulkan.h"
#include <cassert>
#include "../../../../common/print.hpp"

#define SPV_ENABLE_UTILITY_CODE
#include "spirv.hpp"

#include <vector>
#include <unordered_map>

struct SpirvJit {
    public:
    SpirvJit(const uint32_t* data, size_t size);
    
    void load_extension(const std::string& extension);

    std::vector<spv::Capability> capabilities{};
    spv::AddressingModel addressing_model;
    spv::MemoryModel memory_model;

    struct Decoration {
        bool exists;
        uint32_t word;
        std::string string;
    };

    struct Var {
        enum class Type { None, EntryPoint, Extension, Type };
        Type type = Type::None;
        std::string name;
        std::unordered_map<spv::Decoration, Decoration> decorations;
        std::vector<spv::Id> interface;

        struct TypeVar {
            enum class Type { Void, Bool, SInt, UInt, Float, Function, Vector };
            Type type;
            
            union {
                struct {
                    uint32_t width;
                } real;

                struct {
                    uint32_t return_type; // TODO: Id type
                } function;

                struct {
                    size_t n;
                    uint32_t member_type;
                } vector;
            };
        } type_var;

        struct {
            spv::ExecutionModel execution;
            std::string name;
        } entry_point;

        struct {
            struct Member {
                std::string name;
                std::unordered_map<spv::Decoration, Decoration> decorations;
            };

            std::unordered_map<uint32_t, Member> members; // TODO: Store pointers to Var
        } structure;

        struct {
            std::string name;
        } extension;
    };
    std::vector<Var> variables;
    private:
    using Word = uint32_t;
    struct Header {
        Word magic;
        Word version;
        Word generator;
        Word id_bound;
        Word reserved;
    };

    void print_var_list();
};

template<>
struct format::formatter<SpirvJit::Var::Type> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, SpirvJit::Var::Type item){
        switch (item) {
            case SpirvJit::Var::Type::None: it.write("None"); break;
            case SpirvJit::Var::Type::EntryPoint: it.write("Entry Point"); break;
            case SpirvJit::Var::Type::Extension: it.write("Instruction Extension"); break;
            case SpirvJit::Var::Type::Type: it.write("Type variable"); break;
        }
    }
};

template<>
struct format::formatter<SpirvJit::Var::TypeVar::Type> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, SpirvJit::Var::TypeVar::Type item){
        switch (item) {
            case SpirvJit::Var::TypeVar::Type::Void: it.write("Void"); break;
            case SpirvJit::Var::TypeVar::Type::Bool: it.write("Boolean"); break;
            case SpirvJit::Var::TypeVar::Type::SInt: it.write("Signed Integer"); break;
            case SpirvJit::Var::TypeVar::Type::UInt: it.write("Unsigned Integer"); break;
            case SpirvJit::Var::TypeVar::Type::Float: it.write("Floating Point Number"); break;
            case SpirvJit::Var::TypeVar::Type::Vector: it.write("Vector"); break;
            case SpirvJit::Var::TypeVar::Type::Function: it.write("Function"); break;
        }
    }
};

template<>
struct format::formatter<spv::ExecutionModel> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::ExecutionModel item){
        switch (item)
        {
            case spv::ExecutionModel::Vertex: it.write("Vertex"); break;
            case spv::ExecutionModel::Fragment: it.write("Fragment"); break;
            case spv::ExecutionModel::TessellationControl: it.write("Tesselation Control"); break;
            case spv::ExecutionModel::TessellationEvaluation: it.write("Tesselation Eval"); break;
            case spv::ExecutionModel::Geometry: it.write("Geometry"); break;
            case spv::ExecutionModel::GLCompute: it.write("Compute"); break;

            case spv::ExecutionModel::Kernel: it.write("OpenCL Kernel"); break;

            case spv::ExecutionModel::MeshNV: it.write("NV Mesh"); break;
            case spv::ExecutionModel::TaskNV: it.write("NV Task"); break;

            case spv::ExecutionModel::RayGenerationKHR: it.write("KHR Ray Generation"); break;
            case spv::ExecutionModel::MissKHR: it.write("KHR Miss"); break;
            case spv::ExecutionModel::IntersectionKHR: it.write("KHR Intersection"); break;
            case spv::ExecutionModel::ClosestHitKHR: it.write("KHR Closest Hit"); break;
            case spv::ExecutionModel::CallableKHR: it.write("KHR Callable"); break;
            case spv::ExecutionModel::AnyHitKHR: it.write("KHR Any Hit"); break;
            default: break;
        }
    }
};

template<>
struct format::formatter<spv::AddressingModel> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::AddressingModel item){
        switch (item) {
            case spv::AddressingModel::Logical: it.write("Logical"); break;
            case spv::AddressingModel::Physical32: it.write("32 Bit Physical"); break;
            case spv::AddressingModel::Physical64: it.write("64 Bit Physical"); break;
            case spv::AddressingModel::PhysicalStorageBuffer64: it.write("Physical Storage Buffer 64 Bit"); break;
            default: break;
        }
    }
};

template<>
struct format::formatter<spv::MemoryModel> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::MemoryModel item){
        switch (item) {
            case spv::MemoryModel::Simple: it.write("Simple"); break;
            case spv::MemoryModel::GLSL450: it.write("GLSL 4.50"); break;
            case spv::MemoryModel::OpenCL: it.write("OpenCL"); break;
            case spv::MemoryModel::Vulkan: it.write("Vulkan"); break;
            default: break;
        }
    }
};

template<>
struct format::formatter<spv::SourceLanguage> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, spv::SourceLanguage item){
        switch (item) {
            case spv::SourceLanguage::ESSL: it.write("ESSL (OpenGL ES Shading Language)"); break;
            case spv::SourceLanguage::GLSL: it.write("GLSL (OpenGL Shading Language)"); break;
            case spv::SourceLanguage::HLSL: it.write("HLSL (High Level Shader Language)"); break;
            case spv::SourceLanguage::OpenCL_C: it.write("C (OpenCL Kernel)"); break;
            case spv::SourceLanguage::OpenCL_CPP: it.write("C++ (OpenCL Kernel)"); break;
            default: break;
        }
    }
};