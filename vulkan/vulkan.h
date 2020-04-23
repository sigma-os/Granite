#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define VKAPI_ATTR
#define VKAPI_CALL

typedef uint32_t VkBool32;
typedef uint64_t VkDeviceSize;
typedef uint64_t VkDeviceAddress;

typedef uint32_t VkFlags;

typedef enum VkStructureType {
    VK_STRUCTURE_TYPE_APPLICATION_INFO = 0,
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO = 1
} VkStructureType;

typedef enum VkResult {
    VK_SUCCESS = 0,
    VK_NOT_READY = 1,
    VK_TIMEOUT = 2,
    VK_EVENT_SET = 3,
    VK_EVENT_RESET = 4,
    VK_INCOMPLETE = 5,
    VK_ERROR_OUT_OF_HOST_MEMORY = -1,
    VK_ERROR_OUT_OF_DEVICE_MEMORY = -2,
    VK_ERROR_INITIALIZATION_FAILTED = -3,
    VK_ERROR_DEVICE_LOST = -4,
    VK_ERROR_MEMORY_MAP_FAILED = -5,
    VK_ERROR_LAYER_NOT_PRESENT = -7,
    VK_ERROR_EXTENSION_NOT_PRESENT = -8
} VkResult;

typedef struct VkOffset2D {
    int32_t x, y;
} VkOffset2D;

typedef struct VkOffset3D {
    int32_t x, y, z;
} VkOffset3D;

typedef struct VkExtent2D {
    uint32_t width, height;
} VkExtent2D;

typedef struct VkExtent3D {
    uint32_t width, height, depth;
} VkExtent3D;

typedef struct VkRect2D {
    VkOffset2D offset;
    VkExtent2D extent;
} VkRect2D;

#define VK_DEFINE_HANDLE(name) \
    typedef struct name { uint64_t id; } name;

VK_DEFINE_HANDLE(VkInstance);

#define VK_VERSION_MAJOR(version) ((uint32_t)(version) >> 22)
#define VK_VERSION_MINOR(version) (((uint32_t)(version) >> 12) & 0x3FF)
#define VK_VERSION_PATCH(version) ((uint32_t)(version) & 0xFFF)

#define VK_MAKE_VERSION(major, minor, patch) (((major) << 22) | ((minor) << 12) | (patch))
#define VK_API_VERSION_1_0 VK_MAKE_VERSION(1, 0, 0)
#define VK_API_VERSION_1_1 VK_MAKE_VERSION(1, 1, 0)
#define VK_API_VERSION_1_2 VK_MAKE_VERSION(1, 2, 0)

typedef void (*PFN_vkVoidFunction)(void);
PFN_vkVoidFunction vkGetInstanceProcAddr(VkInstance instance, const char* pName);

typedef VkResult (*PFN_vkEnumerateInstanceVersion)(uint32_t*);
VkResult vkEnumerateInstanceVersion(uint32_t* pApiVersion);

#define VK_MAX_EXTENSION_NAME_SIZE 128
#define VK_MAX_DESCRIPTION_SIZE 128

typedef struct VkLayerProperties {
    char layerName[VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t specVersion;
    uint32_t implementationVersion;
    char description[VK_MAX_DESCRIPTION_SIZE];
} VkLayerProperties;

typedef VkResult (*PFN_vkEnumerateInstanceLayerProperties)(uint32_t*, VkLayerProperties*);
VkResult vkEnumerateInstanceLayerProperties(uint32_t* pPropertyCount, VkLayerProperties* pProperties);

typedef struct VkExtensionProperties {
    char extensionName[VK_MAX_EXTENSION_NAME_SIZE];
    uint32_t specVersion;
} VkExtensionProperties;

typedef VkResult (*PFN_vkEnumerateInstanceExtensionProperties)(const char*, uint32_t*, VkExtensionProperties*);
VkResult vkEnumerateInstanceExtensionProperties(const char* pLayerName, uint32_t* pPropertyCount, VkExtensionProperties* pProperties);

typedef enum VkInstanceCreateFlags {
} VkInstanceCreateFlags;

typedef struct VkApplicationInfo {
    VkStructureType sType;
    const void* pNext;
    const char* pApplicationName;
    uint32_t applicationVersion;
    const char* pEngineName;
    uint32_t engineVersion;
    uint32_t apiVersion;
} VkApplicationInfo;

typedef struct VkInstanceCreateInfo {
    VkStructureType sType;
    const void* pNext;
    VkInstanceCreateFlags flags;
    const VkApplicationInfo* pApplicationInfo;
    uint32_t enabledLayerCount;
    const char* const* ppEnabledLayerNames;
    uint32_t enabledExtensionCount;
    const char* const* ppEnabledExtensionNames;
} VkInstanceCreateInfo;

typedef enum VkSystemAllocationScope {
    VK_SYSTEM_ALLOCATION_SCOPE_COMMAND = 0,
    VK_SYSTEM_ALLOCATION_SCOPE_OBJECT = 1,
    VK_SYSTEM_ALLOCATION_SCOPE_CACHE = 2,
    VK_SYSTEM_ALLOCATION_SCOPE_DEVICE = 3,
    VK_SYSTEM_ALLOCATION_SCOPE_INSTANCE = 4
} VkSystemAllocationScope;

typedef enum VkInternalAllocationType {
    VK_INTERNAL_ALLOCATION_TYPE_EXECUTABLE = 0
};

typedef void* (*PFN_vkAllocationFunction)(void*, size_t, size_t, VkSystemAllocationScope);
typedef void* (*PFN_vkReallocationFunction)(void*, void*, size_t, size_t, VkSystemAllocationScope);
typedef void (*PFN_vkFreeFunction)(void*, void*);
typedef void (*PFN_vkInternalAllocationNotification)(void*, size_t, VkInternalAllocationType, VkSystemAllocationScope);
typedef void (*PFN_vkInternalFreeNotification)(void*, size_t, VkInternalAllocationType, VkSystemAllocationScope);

typedef struct VkAllocationCallbacks {
    void* pUserData;
    PFN_vkAllocationFunction pfnAllocation;
    PFN_vkReallocationFunction pfnReallocation;
    PFN_vkFreeFunction pfnFree;
    PFN_vkInternalAllocationNotification pfnInternalAllocation;
    PFN_vkInternalFreeNotification pfnInternalFree;
} VkAllocationCallbacks;

typedef VkResult (*PFN_vkCreateInstance)(const VkInstanceCreateInfo*, const VkAllocationCallbacks*, VkInstance*);
VkResult vkCreateInstance(const VkInstanceCreateInfo* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkInstance* pInstance);


#if defined(__cplusplus)
}
#endif