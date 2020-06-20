#include "meta_ops.hpp"

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
