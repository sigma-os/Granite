#pragma once

#include "../../../common/print.hpp"
#include "../../../vulkan-headers/include/vulkan/vulkan.h"

#include <iterator>
#include <algorithm>

#include "image.hpp"

struct Framebuffer {
    Framebuffer(const VkFramebufferCreateInfo& info): _info{info} {
        assert(info.sType == VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO);
        assert((info.flags & VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT) == 0); // TODO: Support Imageless Framebuffers

        // TODO: Renderpass

        std::copy(info.pAttachments, info.pAttachments + info.attachmentCount, std::back_inserter(_attachments));
    }

    const ImageView& operator[](size_t i) const {
        assert(i < _attachments.size());

        return *_attachments[i];
    }

    ImageView& operator[](size_t i) {
        assert(i < _attachments.size());

        return *_attachments[i];
    }

    size_t n_attachments() const {
        return _attachments.size();
    }

    private:
    VkFramebufferCreateInfo _info;
    std::vector<ImageView*> _attachments;
};