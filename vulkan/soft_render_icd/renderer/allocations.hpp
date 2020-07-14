#pragma once

#include "../../../common/print.hpp"
#include "../../../vulkan-headers/include/vulkan/vulkan.h"

#include <array>
#include <memory>

struct Memory {
    struct MemorySlice {
        MemorySlice() = default;
        MemorySlice(Memory* memory, uintptr_t off, size_t size): _memory{memory}, _off{off}, _size{size} {}

        uint8_t& operator[](size_t i){
            assert(i < _size);

            return (*_memory)[_off + i];
        }

        const uint8_t& operator[](size_t i) const {
            assert(i < _size);

            return (*_memory)[_off + i];
        }

        MemorySlice subslice(uintptr_t off, size_t size){
            assert(size < _size);
            return MemorySlice{_memory, _off + off, size};
        }

        void* addr(uintptr_t off = 0){
            return _memory->addr(_off + off);
        }

        private:
        Memory* _memory;

        uintptr_t _off;
        size_t _size;
    };


    Memory(const VkMemoryAllocateInfo& info): _info{info} {
        assert(info.sType == VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);

        _data = std::make_unique<uint8_t[]>(info.allocationSize);
    }
    
    uint8_t& operator[](size_t i){
        return _data[i];
    }

    const uint8_t& operator[](size_t i) const {
        return _data[i];
    }

    MemorySlice global_slice(){
        return MemorySlice{this, 0, _info.allocationSize};
    }

    void* addr(uintptr_t off = 0){
        return (void*)((uintptr_t)_data.get() + off);
    }

    private:
    VkMemoryAllocateInfo _info;

    std::unique_ptr<uint8_t[]> _data;
};

using MemorySlice = Memory::MemorySlice;