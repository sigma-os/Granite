#include "../vulkan-headers/include/vulkan/vulkan.h"
#include <stdio.h>
#include <vector>

#define CHECK_FAILURE(expr) \
    if(e = (expr); e != VK_SUCCESS){ \
        printf(#expr " failed, error = %x\n", e); \
        return 1; \
    }

int main(){
    uint32_t version = 0;
    VkResult e = VK_SUCCESS;
    CHECK_FAILURE(vkEnumerateInstanceVersion(&version));
    printf("API Version: %d.%d.%d\n", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));

    uint32_t n_layers = 0;
    CHECK_FAILURE(vkEnumerateInstanceLayerProperties(&n_layers, NULL));
    printf("Number of layers: %d\n", n_layers);

    std::vector<VkLayerProperties> layers{};
    layers.resize(n_layers);
    CHECK_FAILURE(vkEnumerateInstanceLayerProperties(&n_layers, layers.data()));

    for(const auto& layer : layers)
        printf("\t- Layer: name: %s description: %s spec: %d.%d.%d impl: %d\n", layer.layerName, layer.description, VK_VERSION_MAJOR(layer.specVersion), VK_VERSION_MINOR(layer.specVersion), VK_VERSION_PATCH(layer.specVersion), layer.implementationVersion);


    uint32_t n_extensions = 0;
    CHECK_FAILURE(vkEnumerateInstanceExtensionProperties(NULL, &n_extensions, NULL));
    printf("Number of extensions: %d\n", n_extensions);

    std::vector<VkExtensionProperties> extensions{};
    extensions.resize(n_extensions);
    CHECK_FAILURE(vkEnumerateInstanceExtensionProperties(NULL, &n_extensions, extensions.data()));

    for(const auto& extension : extensions)
        printf("\t- Extension: name: %s spec: %d.%d.%d\n", extension.extensionName, VK_VERSION_MAJOR(extension.specVersion), VK_VERSION_MINOR(extension.specVersion), VK_VERSION_PATCH(extension.specVersion));

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_MAKE_VERSION(1, 2, 0);

    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;

    std::vector<const char*> requested_layers{};
    requested_layers.push_back("VK_GRANITE_layer_validation");

    std::vector<const char*> requested_extensions{};
    requested_extensions.push_back("VK_SIGMA_wm_surface");

    instance_info.enabledLayerCount = requested_layers.size();
    instance_info.ppEnabledLayerNames = requested_layers.data();

    instance_info.enabledExtensionCount = requested_extensions.size();
    instance_info.ppEnabledExtensionNames = requested_extensions.data();

    VkInstance instance;

    CHECK_FAILURE(vkCreateInstance(&instance_info, NULL, &instance));
    printf("Created Instance\n");

    return 0;
}