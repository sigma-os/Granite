#include "jit.hpp"
#include <unordered_map>
#include <string_view>

using OpcodeFunction = void (*)(SpirvJit&, uint32_t, const uint32_t*);

void execute_OpSource([[maybe_unused]] SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto lang = (spv::SourceLanguage)data[0];
    auto version = data[1];

    print("OpSource: {}, v{:d}\n", lang, version);
}

void execute_OpSourceExtension([[maybe_unused]] SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto length = strlen((const char*)data);
    std::string str{(const char*)data, length};
    print("OpSourceExtension: {}\n", str);
}

void execute_OpName(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];

    auto length = strlen((const char*)(data + 1));
    std::string str{(const char*)(data + 1), length};

    code.variables[id].name = std::move(str);
}

void execute_OpMemberName(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto member = data[1];

    auto length = strlen((const char*)(data + 2));
    std::string str{(const char*)(data + 2), length};

    code.variables[id].structure.members[member].name = std::move(str);
}


void execute_OpExtension(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto length = strlen((const char*)data);
    std::string str{(const char*)data, length};
    print("OpExtension: {}\n", str);

    code.load_extension(str);
}

void execute_OpExtInstImport(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto length = strlen((const char*)(data + 1));
    std::string str{(const char*)(data + 1), length};
    print("OpExtInstImport: Name: {}\n", str);

    code.variables[id].type = SpirvJit::Var::Type::Extension;
    code.variables[id].extension.name = std::move(str);
}

void execute_OpMemoryModel(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    code.addressing_model = (spv::AddressingModel)data[0];
    code.memory_model = (spv::MemoryModel)data[1];

    print("OpMemoryModel: Addressing: {:s}, Memory: {:s}\n", code.addressing_model, code.memory_model);
}

void execute_OpEntryPoint(SpirvJit& code, uint32_t instruction_len, const uint32_t* data){
    auto execution_model = (spv::ExecutionModel)data[0];
    auto id = data[1];

    auto length = strlen((const char*)(data + 2));
    std::string name{(const char*)(data + 2), length};

    print("OpEntryPoint: Name: {}, Execution Model: {}\n", name, execution_model);

    auto& var = code.variables[id];
    var.type = SpirvJit::Var::Type::EntryPoint;

    var.entry_point.execution = execution_model;
    var.entry_point.name = std::move(name);

    size_t off = ((length + 4 - 1) / 4) + 3; // TODO: Sure this is correct?
    for(; off < (instruction_len - 1); off++)
        var.interface.push_back(data[off]);
}

void execute_OpCapability(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto cap = (spv::Capability)data[0];

    code.capabilities.push_back(cap);

    switch (cap) {
    default:
        print("Unknown spv::Capability {:d}\n", data[0]);
        break;
    }
}

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
    code.variables[id].type_var.vector.n = width;
    code.variables[id].type_var.vector.member_type = type;
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

static void parse_decoration(SpirvJit::Decoration& to, spv::Decoration decoration, const uint32_t* data){
    if (decoration == spv::Decoration::SpecId || decoration == spv::Decoration::ArrayStride || decoration == spv::Decoration::MatrixStride || decoration == spv::Decoration::BuiltIn || 
        decoration == spv::Decoration::UniformId || decoration == spv::Decoration::Stream || decoration == spv::Decoration::Location || decoration == spv::Decoration::Component ||
        decoration == spv::Decoration::Index || decoration == spv::Decoration::Binding || decoration == spv::Decoration::DescriptorSet || decoration == spv::Decoration::Offset || 
        decoration == spv::Decoration::XfbBuffer || decoration == spv::Decoration::XfbStride || decoration == spv::Decoration::FuncParamAttr || decoration == spv::Decoration::FPRoundingMode || 
        decoration == spv::Decoration::FPFastMathMode || decoration == spv::Decoration::InputAttachmentIndex || decoration == spv::Decoration::Alignment || decoration == spv::Decoration::MaxByteOffset || 
        decoration == spv::Decoration::AlignmentId || decoration == spv::Decoration::MaxByteOffsetId || decoration == spv::Decoration::SecondaryViewportRelativeNV || decoration == spv::Decoration::CounterBuffer || 
        decoration == spv::Decoration::HlslCounterBufferGOOGLE || decoration == spv::Decoration::XfbStride) {

        // 1 Word
        to.exists = true;
        to.word = data[0];
    } else if (decoration == spv::Decoration::LinkageAttributes) {
        // 1 String + 1 Word
        throw std::runtime_error("parse_decoration: Implement strings");
    } else if (decoration == spv::Decoration::UserSemantic || decoration == spv::Decoration::HlslSemanticGOOGLE || decoration == spv::Decoration::UserTypeGOOGLE) {
        // 1 String
        throw std::runtime_error("parse_decoration: Implement strings");
    } else {
        // No Attributes
        to.exists = true;
    }
}

void execute_OpDecorate(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto decoration = (spv::Decoration)data[1];

    auto& decorations = code.variables[id].decorations;

    parse_decoration(decorations[decoration], decoration, data + 2);
}

void execute_OpMemberDecorate(SpirvJit& code, [[maybe_unused]] uint32_t instruction_len, const uint32_t* data){
    auto id = data[0];
    auto member = data[1];
    auto decoration = (spv::Decoration)data[2];

    auto& decorations = code.variables[id].structure.members[member].decorations;

    parse_decoration(decorations[decoration], decoration, data + 3);
}

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

    {spv::Op::OpTypeFunction, execute_OpTypeFunction},

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
                    default: break;
                    case Var::TypeVar::Type::UInt: [[fallthrough]];
                    case Var::TypeVar::Type::SInt: [[fallthrough]];
                    case Var::TypeVar::Type::Float: print("\t- Width: {:d}\n", var.type_var.real.width); break;
                    case Var::TypeVar::Type::Vector: print("\t- {:d} Elements of type {:d}\n", var.type_var.vector.n, var.type_var.vector.member_type); break;
                    case Var::TypeVar::Type::Function: print("\t- Return type: {:d}\n", var.type_var.function.return_type); break;
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
        }
    }
}