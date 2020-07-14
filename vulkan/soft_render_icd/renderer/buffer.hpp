#pragma once

#include "../../../common/print.hpp"
#include "../../../vulkan-headers/include/vulkan/vulkan.h"

#include <array>
#include <memory>

#include "allocations.hpp"

struct Buffer {
    Buffer(const VkBufferCreateInfo& info): _info{info} {
        assert(info.sType == VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO);

        assert(info.flags == 0);
        assert(info.sharingMode == VK_SHARING_MODE_EXCLUSIVE);
    }

    VkMemoryRequirements get_requirements(){
        VkMemoryRequirements ret{};
        ret.size = _info.size;
        ret.alignment = 4; // TODO? What should I fill in here
        ret.memoryTypeBits = 0;

        return ret;
    }

    void bind(Memory& memory, uintptr_t off){
        _slice = memory.global_slice().subslice(off, _info.size);
    }

    void* map(uintptr_t off){
        return _slice.addr(off);
    }

    void* addr(uintptr_t off = 0){
        return _slice.addr(off);
    }

    void unmap(){
        // Not needed right now
    }

    private:
    VkBufferCreateInfo _info;
    MemorySlice _slice;
};