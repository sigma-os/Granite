#include "vulkan.h"
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

    return 0;
}