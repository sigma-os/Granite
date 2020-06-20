#include "type_ops.hpp"

void execute_OpTypeVoid(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    code.variables[id].type = SpirvJit::Var::Type::Type;
    code.variables[id].type_var.type = SpirvJit::Var::TypeVar::Type::Void;
}

void execute_OpTypeBool(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    code.variables[id].type = SpirvJit::Var::Type::Type;
    code.variables[id].type_var.type = SpirvJit::Var::TypeVar::Type::Bool;
}

void execute_OpTypeInt(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto width = data[1];
    auto signedness = data[2];

    code.variables[id].type = SpirvJit::Var::Type::Type;
    code.variables[id].type_var.type = (signedness == 0) ? SpirvJit::Var::TypeVar::Type::UInt : SpirvJit::Var::TypeVar::Type::SInt;
    code.variables[id].type_var.real.width = width;
}

void execute_OpTypeFloat(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto width = data[1];

    code.variables[id].type = SpirvJit::Var::Type::Type;
    code.variables[id].type_var.type = SpirvJit::Var::TypeVar::Type::Float;
    code.variables[id].type_var.real.width = width;
}

void execute_OpTypeVector(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto type = data[1];
    auto width = data[2];

    code.variables[id].type = SpirvJit::Var::Type::Type;
    code.variables[id].type_var.type = SpirvJit::Var::TypeVar::Type::Vector;
    code.variables[id].type_var.composite.n = width;
    code.variables[id].type_var.composite.member_type = type;
}

void execute_OpTypeArray(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto type = data[1];
    auto width = data[2];

    assert(code.variables[width].type == SpirvJit::Var::Type::Constant);
    assert(code.variables[code.variables[width].constant.type].type == SpirvJit::Var::Type::Type);
    assert(code.variables[code.variables[width].constant.type].type_var.type == SpirvJit::Var::TypeVar::Type::UInt);

    code.variables[id].type = SpirvJit::Var::Type::Type;
    code.variables[id].type_var.type = SpirvJit::Var::TypeVar::Type::Array;
    code.variables[id].type_var.composite.n = code.variables[width].constant.unsigned_int;
    code.variables[id].type_var.composite.member_type = type;
}

void execute_OpTypeFunction(SpirvJit& code, uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto return_type = data[1];

    code.variables[id].type = SpirvJit::Var::Type::Type;
    code.variables[id].type_var.type = SpirvJit::Var::TypeVar::Type::Function;
    code.variables[id].type_var.function.return_type = return_type;

    for(size_t i = 2; i < (instruction_len - 1); i++)
        code.variables[id].interface.push_back(data[i]);
}

void execute_OpConstant(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[1];
    auto type = data[0];

    assert(code.variables[type].type == SpirvJit::Var::Type::Type);

    assert(code.variables[type].type_var.real.width == 32); // TODO: Implement other widths

    code.variables[id].type = SpirvJit::Var::Type::Constant;
    code.variables[id].constant.type = type;

    switch (code.variables[type].type_var.type) {
        case SpirvJit::Var::TypeVar::Type::SInt: code.variables[id].constant.signed_int = (int32_t)data[2]; break;
        case SpirvJit::Var::TypeVar::Type::UInt: code.variables[id].constant.unsigned_int = (uint32_t)data[2]; break;
        case SpirvJit::Var::TypeVar::Type::Float: code.variables[id].constant.real = *(float*)&data[2]; break;
        default: assert(!"Invalid type in OpConstant");
    }
}