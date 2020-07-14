#pragma once

#include "../../../common/print.hpp"
#include "../../../vulkan-headers/include/vulkan/vulkan.h"

#include <array>
#include <memory>
#include <vector>

#include "allocations.hpp"
#include "buffer.hpp"

struct Image {
    Image(const VkImageCreateInfo& info): _info{info} {
        assert(info.sType == VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO);

        assert(info.flags == 0); // TODO
        assert(info.imageType == VK_IMAGE_TYPE_2D); // TODO
        assert(info.sharingMode == VK_SHARING_MODE_EXCLUSIVE);
        assert(info.samples == VK_SAMPLE_COUNT_1_BIT);
        
        assert(format_is_supported(info.format));

        
    }

    VkMemoryRequirements get_requirements(){
        VkMemoryRequirements ret{};
        ret.size = _info.extent.width * _info.extent.height * _info.extent.depth;
        ret.alignment = 4; // TODO? What should I fill in here
        ret.memoryTypeBits = 0;

        return ret;
    }

    void bind(Memory* mem, uintptr_t off){
        _slice = mem->global_slice().subslice(off, get_requirements().size);
    }

    void copy_from_buffer(Buffer& buf, const std::vector<VkBufferImageCopy>& regions){
        for(const auto& region : regions){
            size_t i = ((_info.extent.width * _info.extent.height) * region.imageOffset.z) + (_info.extent.width * region.imageOffset.y) + region.imageOffset.x;
            size_t size = region.imageExtent.width * region.imageExtent.height * region.imageExtent.depth;
            
            memcpy(_slice.addr(i), buf.addr(region.bufferOffset), size);
        }
    }

    private:
    static bool format_is_supported(VkFormat format){
        for(const auto check : _supported_formats)
            if(format == check)
                return true;

        return false;
    }

    VkImageCreateInfo _info;
    MemorySlice _slice;

    static constexpr std::array<VkFormat, 1> _supported_formats = {
        VK_FORMAT_R8G8B8A8_UNORM
    };
};

struct ImageView {
    ImageView(const VkImageViewCreateInfo& info): _info{info} {
        assert(info.sType == VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO);

        assert(info.viewType == VK_IMAGE_VIEW_TYPE_2D); // TODO

        _image = nullptr; // TODO: Retrieve in some kind of way `info.image`
    }

    private:
    VkImageViewCreateInfo _info;

    Image* _image;
};