#pragma once

#include "../../../../vulkan-headers/include/vulkan/vulkan.h"
#include <cassert>
#include "../../../../common/print.hpp"

#include "spirv.hpp"
#include "spirv_print.hpp"

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
        enum class Type { None, EntryPoint, Extension, Type, Constant };
        Type type = Type::None;
        std::string name;
        std::unordered_map<spv::Decoration, Decoration> decorations;
        std::vector<spv::Id> interface;

        struct TypeVar {
            enum class Type { Void, Bool, SInt, UInt, Float, Function, Vector, Array };
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
                } composite;
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

        struct {
            uint32_t type;
            union {
                uint32_t unsigned_int;
                int32_t signed_int;
                float real;

                float vec[4];
                double dvec[4];
            };
        } constant;
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
            case SpirvJit::Var::Type::Constant: it.write("Constant"); break;
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
            case SpirvJit::Var::TypeVar::Type::Array: it.write("Array"); break;
            case SpirvJit::Var::TypeVar::Type::Function: it.write("Function"); break;
        }
    }
};