#include "../vulkan.h"
#include <cassert>
#include <array>
#include <unordered_map>
#include <cstring>

namespace granite::vulkan
{
    struct handle {};
    std::unordered_map<uint64_t, handle*> handles;

    VkResult enumerate_instance_version(uint32_t* pApiVersion){
        assert(pApiVersion);
        *pApiVersion = VK_API_VERSION_1_2;
        return VK_SUCCESS;
    }

    VkResult enumerate_instance_layer_properties(uint32_t* pPropertyCount, VkLayerProperties* pProperties){
        assert(pPropertyCount);

        std::array<VkLayerProperties, 2> layers = { // Dummies
            VkLayerProperties{"VK_GRANITE_layer_validation", VK_MAKE_VERSION(1, 2, 0), 1, ""},
            VkLayerProperties{"VK_GRANITE_layer_overlay", VK_MAKE_VERSION(1, 2, 0), 1, ""}
        };

        if(pProperties){
            assert(*pPropertyCount <= layers.size());
            memcpy(pProperties, layers.data(), *pPropertyCount * sizeof(VkLayerProperties));

            VkResult result = (*pPropertyCount < layers.size()) ? VK_INCOMPLETE : VK_SUCCESS;
            *pPropertyCount = layers.size();
            return result;
        } else {
            *pPropertyCount = layers.size();
            return VK_SUCCESS;
        }
    }

    VkResult enumerate_instance_extension_properties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties){
        assert(pLayerName == nullptr); // This is in a driver so layer should always be null
        assert(pPropertyCount);

        std::array<VkExtensionProperties, 4> extensions = { // Dummies
            VkExtensionProperties{"VK_KHR_display", VK_MAKE_VERSION(0, 0, 1)},
            VkExtensionProperties{"VK_KHR_surface", VK_MAKE_VERSION(0, 0, 1)},
            VkExtensionProperties{"VK_SIGMA_wm_surface", VK_MAKE_VERSION(0, 0, 1)},
            VkExtensionProperties{"VK_EXT_debug_utils", VK_MAKE_VERSION(0, 0, 1)}
        };

        if(pProperties){
            assert(*pPropertyCount <= extensions.size());
            memcpy(pProperties, extensions.data(), *pPropertyCount * sizeof(VkExtensionProperties));

            VkResult result = (*pPropertyCount < extensions.size()) ? VK_INCOMPLETE : VK_SUCCESS;
            *pPropertyCount = extensions.size();
            return result;
        } else {
            *pPropertyCount = extensions.size();
            return VK_SUCCESS;
        }
    }
} // namespace granite

extern "C" {
    VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(uint32_t* pApiVersion){
        return granite::vulkan::enumerate_instance_version(pApiVersion);
    }
    
    VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties){
        return granite::vulkan::enumerate_instance_layer_properties(pPropertyCount, pProperties);
    }

    VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties){
        return granite::vulkan::enumerate_instance_extension_properties(pLayerName, pPropertyCount, pProperties);
    }
}


