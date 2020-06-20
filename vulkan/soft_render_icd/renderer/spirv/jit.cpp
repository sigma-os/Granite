#include "jit.hpp"

#include <unordered_map>
#include <string_view>

using OpcodeFunction = void (*)(SpirvJit&, uint32_t, const uint32_t*);

#include "ops/meta_ops.hpp"
#include "ops/type_ops.hpp"

std::unordered_map<spv::Op, OpcodeFunction> opcode_map = {
    {spv::Op::OpSource, execute_OpSource},
    {spv::Op::OpSourceExtension, execute_OpSourceExtension},
    {spv::Op::OpName, execute_OpName},
    {spv::Op::OpMemberName, execute_OpMemberName},

    {spv::Op::OpExtension, execute_OpExtension},
    {spv::Op::OpExtInstImport, execute_OpExtInstImport},

    {spv::Op::OpMemoryModel, execute_OpMemoryModel},
    {spv::Op::OpEntryPoint, execute_OpEntryPoint},
    {spv::Op::OpCapability, execute_OpCapability},

    {spv::Op::OpTypeVoid, execute_OpTypeVoid},
    {spv::Op::OpTypeBool, execute_OpTypeBool},
    {spv::Op::OpTypeInt, execute_OpTypeInt},
    {spv::Op::OpTypeFloat, execute_OpTypeFloat},
    {spv::Op::OpTypeVector, execute_OpTypeVector},

    {spv::Op::OpTypeArray, execute_OpTypeArray},

    {spv::Op::OpTypeFunction, execute_OpTypeFunction},

    {spv::Op::OpConstant, execute_OpConstant},

    {spv::Op::OpDecorate, execute_OpDecorate},
    {spv::Op::OpMemberDecorate, execute_OpMemberDecorate}
};


SpirvJit::SpirvJit(const uint32_t* data, size_t size){
    const auto& header = *(Header*)data;
    assert(header.magic == spv::MagicNumber);

    print("Parsing SPIR-V Shader code:\n");
    print("SPIR-V Version: {:d}.{:d}\n", header.version >> 16, header.version >> 8);
    print("Generator: {:d}, v{:d}\n", header.generator >> 16, header.generator & 0xFFFF);
    print("SSA ID Bounds: 0 < id < {:d}\n\n", header.id_bound);

    variables.resize(header.id_bound);

    const uint32_t* instructions = data + 5; // Size of header
    const uint32_t* limit = data + (size / 4);
    while(instructions < limit){
        auto opcode = instructions[0];
        auto op = (spv::Op)(opcode & spv::OpCodeMask);
        auto len = (opcode & ~spv::OpCodeMask) >> spv::WordCountShift;

        if(opcode_map.count(op) == 0){
            print_var_list();
            print("Unimplemented Opcode: {:d}\n", (uint32_t)op);
            throw std::runtime_error("JIT: Unimplemented Opcode");
        }
        opcode_map[op](*this, len, instructions + 1);

        instructions += len;
    }
}

void SpirvJit::load_extension(const std::string& extension){
    print("Unknown extension: {}\n", extension);
}

void SpirvJit::print_var_list(){
    print("Printing SSA var list:\n");
    for(size_t i = 0; i < variables.size(); i++){
        const auto& var = variables[i];
        if(var.type == Var::Type::None)
            continue;

        print("%{:d}:\n", i);
        if(var.name.size()) 
            print("\t- Name: \"{:s}\"\n", variables[i].name);

        switch (variables[i].type) {
            case Var::Type::None:
                break;
            case Var::Type::Type:
                print("\t- Type: Type variable\n");
                print("\t- Member Type: {}\n", var.type_var.type);
                switch(var.type_var.type){
                    case Var::TypeVar::Type::UInt: [[fallthrough]];
                    case Var::TypeVar::Type::SInt: [[fallthrough]];
                    case Var::TypeVar::Type::Float: print("\t- Width: {:d}\n", var.type_var.real.width); break;
                    case Var::TypeVar::Type::Array: [[fallthrough]];
                    case Var::TypeVar::Type::Vector: print("\t- {:d} Elements of type {:d}\n", var.type_var.composite.n, var.type_var.composite.member_type); break;
                    case Var::TypeVar::Type::Function: print("\t- Return type: {:d}\n", var.type_var.function.return_type); break;
                    default: break;
                }
                break;
            case Var::Type::Extension:
                print("\t- Type: Instruction extension\n");
                print("\t- Extension name: {:s}\n", var.extension.name);
                break;
            case Var::Type::EntryPoint:
                print("\t- Type: Entry Point\n");
                print("\t- Entry Point Execution Mode: {:d}\n", var.entry_point.execution);
                print("\t- Entry Point Name: \"{}\"\n", var.entry_point.name);
                print("\t- Entry Point Interface:");
                for(const auto id : var.interface)
                    print(" %{:d}", id);
                print("\n");
                break;
            case Var::Type::Constant:
                print("\t- Type: Constant\n");
                print("\t- Constant Type: {}\n", variables[var.constant.type].type_var.type);
                switch (variables[var.constant.type].type_var.type)
                {
                    case Var::TypeVar::Type::UInt: print("\t- Value: {}\n", var.constant.unsigned_int); break;
                    case Var::TypeVar::Type::SInt: print("\t- Value: {}\n", var.constant.signed_int); break;
                    case Var::TypeVar::Type::Float: print("\t- value: //TODO: Float printing\n"); break;
                }
        }
    }
}