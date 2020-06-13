#include "jit.hpp"

#include <fstream>
#include <stdexcept>

#include <cstdint>
#include <cstddef>

std::vector<std::byte> read_binary_file(const std::string& name){
    std::ifstream file{name, std::ios::ate | std::ios::binary};

    if(!file.is_open())
        throw std::runtime_error("Failed to open file");

    size_t size = file.tellg();
    std::vector<std::byte> buf{size};

    file.seekg(0);
    file.read((char*)buf.data(), buf.size());

    file.close();
    return buf;
}

int main(){
    auto code = read_binary_file("../vulkan/soft_render_icd/renderer/spirv/test.spv");
    SpirvJit jit{(const uint32_t*)code.data(), code.size()};
}