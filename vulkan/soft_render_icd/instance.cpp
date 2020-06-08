#include "../../../vulkan-headers/include/vulkan/vulkan.h"
#include "../../common/print.hpp"
#include <cassert>
#include <array>
#include <unordered_map>
#include <cstring>

namespace granite::vulkan
{
    struct spec_version {
        spec_version(uint32_t v): major{VK_VERSION_MAJOR(v)}, minor{VK_VERSION_MINOR(v)}, patch{VK_VERSION_PATCH(v)} {}
        uint32_t major, minor, patch;
    };
}

template<>
struct format::formatter<granite::vulkan::spec_version> {
	template<typename OutputIt>
	static void format(format::format_output_it<OutputIt>& it, [[maybe_unused]] format::format_args args, granite::vulkan::spec_version item){        
		formatter<uintptr_t>::format(it, {}, item.major);
        it.write('.');
        formatter<uintptr_t>::format(it, {}, item.minor);
        it.write('.');
        formatter<uintptr_t>::format(it, {}, item.patch);
    }
};

namespace granite::vulkan
{
    constexpr std::array<VkExtensionProperties, 4> supported_extensions = { // Dummies
        VkExtensionProperties{"VK_KHR_display", VK_MAKE_VERSION(0, 0, 1)},
        VkExtensionProperties{"VK_KHR_surface", VK_MAKE_VERSION(0, 0, 1)},
        VkExtensionProperties{"VK_SIGMA_wm_surface", VK_MAKE_VERSION(0, 0, 1)},
        VkExtensionProperties{"VK_EXT_debug_utils", VK_MAKE_VERSION(0, 0, 1)}
    };

    constexpr std::array<VkLayerProperties, 2> supported_layers = { // Dummies
        VkLayerProperties{"VK_GRANITE_layer_validation", VK_MAKE_VERSION(1, 2, 0), 1, ""},
        VkLayerProperties{"VK_GRANITE_layer_overlay", VK_MAKE_VERSION(1, 2, 0), 1, ""}
    };

    VkResult enumerate_instance_version(uint32_t* pApiVersion){
        assert(pApiVersion);
        *pApiVersion = VK_API_VERSION_1_2;
        return VK_SUCCESS;
    }

    VkResult enumerate_instance_layer_properties(uint32_t* pPropertyCount, VkLayerProperties* pProperties){
        assert(pPropertyCount);

        if(pProperties){
            assert(*pPropertyCount <= supported_layers.size());
            memcpy(pProperties, supported_layers.data(), *pPropertyCount * sizeof(VkLayerProperties));

            VkResult result = (*pPropertyCount < supported_layers.size()) ? VK_INCOMPLETE : VK_SUCCESS;
            *pPropertyCount = supported_layers.size();
            return result;
        } else {
            *pPropertyCount = supported_layers.size();
            return VK_SUCCESS;
        }
    }

    VkResult enumerate_instance_extension_properties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties){
        assert(pLayerName == nullptr); // This is in a driver so layer should always be null
        assert(pPropertyCount);

        if(pProperties){
            assert(*pPropertyCount <= supported_extensions.size());
            memcpy(pProperties, supported_extensions.data(), *pPropertyCount * sizeof(VkExtensionProperties));

            VkResult result = (*pPropertyCount < supported_extensions.size()) ? VK_INCOMPLETE : VK_SUCCESS;
            *pPropertyCount = supported_extensions.size();
            return result;
        } else {
            *pPropertyCount = supported_extensions.size();
            return VK_SUCCESS;
        }
    }

    struct Instance {
        uint32_t version;
    };

    std::unordered_map<uint64_t, Instance> instances;
    VkResult create_instance(const VkInstanceCreateInfo* pCreateInfo, [[maybe_unused]] const VkAllocationCallbacks* pAllocator, VkInstance* pInstance){
        static auto gen_id = [curr = 0ull]() mutable -> uint64_t {
            return curr++;
        };

        assert(pInstance);
        assert(pCreateInfo);

        uint32_t selected_version = 0;
        assert(enumerate_instance_version(&selected_version) == VK_SUCCESS);

        if(pCreateInfo->pApplicationInfo){
            if(pCreateInfo->pApplicationInfo->apiVersion <= selected_version)
                selected_version = pCreateInfo->pApplicationInfo->apiVersion;
            else 
                print("SOFT_RENDER_ICD: Requested unsupported Vulkan version: {}, taking default ({})\n", spec_version{pCreateInfo->pApplicationInfo->apiVersion}, spec_version{selected_version});
        }

        for(size_t i = 0; i < pCreateInfo->enabledLayerCount; i++){
            const auto* layer_name = pCreateInfo->ppEnabledLayerNames[i];
            
            bool found = false;
            for(const auto& layer : supported_layers){
                if(strcmp(layer_name, layer.layerName) == 0){
                    found = true;
                    break;
                }
            }

            if(!found){
                print("SOFT_RENDER_ICD: Couldn't find requested layer {:d}\n", layer_name);
                return VK_ERROR_LAYER_NOT_PRESENT;
            }
        }

        for(size_t i = 0; i < pCreateInfo->enabledExtensionCount; i++){
            const auto* extension_name = pCreateInfo->ppEnabledExtensionNames[i];
            
            bool found = false;
            for(const auto& extension : supported_extensions){
                if(strcmp(extension_name, extension.extensionName) == 0){
                    found = true;
                    break;
                }
            }

            if(!found){
                print("SOFT_RENDER_ICD: Couldn't find requested extension {:d}\n", extension_name);
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        auto id = gen_id();
        *pInstance = (VkInstance)id;
        auto& instance = instances[id];
        instance.version = selected_version;

        return VK_SUCCESS;
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

    VKAPI_ATTR VkResult VKAPI_CALL vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance){
        return granite::vulkan::create_instance(pCreateInfo, pAllocator, pInstance);
    }
}