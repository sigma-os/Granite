#include <dlfcn.h>
#include <stdlib.h>
#include <stdio.h>
#include "../vulkan.h"
#include "../common/print.hpp"

#define STRCAT_4(a, b, c, d) a # b # c # d
#define STRCAT_3(a, b, c) a # b # c

namespace {
    
    template<typename... Args>
    [[noreturn]]
    void panic(const char* reason, Args&&... args){
        print("VK Loader: Panic -> ");
        print(reason, std::forward<Args>(args)...);
        print("\n");
        abort();
    }

    class Loader {
        public:
        Loader(){
            const char* driver = "soft_render_icd/libvulkan_granite.so";//"/usr/lib/libvulkan_granite.so";
            driver_handle = dlopen(driver, RTLD_LAZY);

            if(!driver_handle)
                panic("Couldn't open driver: {:s} error: {:d}", driver, dlerror());
        }

        ~Loader(){
            dlclose(driver_handle);
        }

        PFN_vkVoidFunction get_proc_addr(const char* name){
            return (PFN_vkVoidFunction)dlsym(driver_handle, name);
        }

        private:
        void* driver_handle;
    };

    Loader global_loader;
}

extern "C" {
    VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceVersion(uint32_t* pApiVersion){
        static auto* f = (PFN_vkEnumerateInstanceVersion)global_loader.get_proc_addr("vkEnumerateInstanceVersion");
        if(f)
            return f(pApiVersion);
        panic("Couldn't find {:s} in loaded driver", __func__);
    }

    VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties){
        static auto* f = (PFN_vkEnumerateInstanceLayerProperties)global_loader.get_proc_addr("vkEnumerateInstanceLayerProperties");
        if(f)
            return f(pPropertyCount, pProperties);
        panic("Couldn't find {:s} in loaded driver", __func__);
    }

    VKAPI_ATTR VkResult VKAPI_CALL vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties){
        static auto* f = (PFN_vkEnumerateInstanceExtensionProperties)global_loader.get_proc_addr("vkEnumerateInstanceExtensionProperties");
        if(f)
            return f(pLayerName, pPropertyCount, pProperties);
        panic("Couldn't find {:s} in loaded driver", __func__);
    }
}
