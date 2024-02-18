#include "VulkanContext.h"
#include <Core\Log.h>
#include <Platform\Common\Types.h>
#include "VulkanTypes.h"

VulkanContext::VulkanContext()
{
    device = new VulkanDevice();
}

VulkanContext::~VulkanContext()
{
    delete device;
    LOG_TRACE("Destroying Vulkan surface...");
    vkDestroySurfaceKHR(instance, surface, allocator);
#if defined(_DEBUG)
    LOG_TRACE("Destroying Vulkan debugger...");
    if (debugMessenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        func(instance, debugMessenger, allocator);
    }
#endif
    LOG_TRACE("Destroying Vulkan instance...");
    vkDestroyInstance(instance, allocator);
}

bool VulkanContext::Initialize(WindowHandle_t windowHandle)
{
    allocator = nullptr;

    // Setup Vulkan instance.
    VkApplicationInfo app_info = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
    app_info.apiVersion = VK_API_VERSION_1_2;
    app_info.pApplicationName = ""; //TODO:configure this
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Juicy Engine";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

    VkInstanceCreateInfo create_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    create_info.pApplicationInfo = &app_info;

    std::vector<const achar*>	requiredExtensions;

    uint32			platformExtensionCount = 0;
    const achar** platformExtensionsRequired = VK_Plat_GetRequiredInstanceExtensions(platformExtensionCount);
    for (uint32 index = 0; index < platformExtensionCount; ++index)
    {
        requiredExtensions.push_back(platformExtensionsRequired[index]);
    }
    requiredExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
#if defined(_DEBUG)
    requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    std::vector<const achar*> requiredValidationLayerName;


#if defined(_DEBUG)
    requiredValidationLayerName.push_back("VK_LAYER_KHRONOS_validation");
    LOG_TRACE("Validation layers enabled. Enumerating...");


    // Obtain a list of available validation layers
    uint32 availableLayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, 0));
    std::vector<VkLayerProperties> availableLayers(availableLayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data()));

    // Verify all required layers are available.
    for (uint32 i = 0; i < requiredValidationLayerName.size(); ++i) {
        LOG_TRACE("Searching for layer: {0}", requiredValidationLayerName[i]);
        bool found = false;
        for (uint32 j = 0; j < availableLayerCount; ++j) {
            if (std::strcmp(requiredValidationLayerName[i], availableLayers[j].layerName) == 0) {
                found = TRUE;
                LOG_TRACE("Found. {0}", requiredValidationLayerName[i]);
                break;
            }
        }

        if (!found) {
            LOG_CRITICAL("Required validation layer is missing: {0}", requiredValidationLayerName[i]);
            return false;
        }
    }
    LOG_TRACE("All required validation layers are present.");
#endif


    create_info.enabledExtensionCount = requiredExtensions.size();
    create_info.ppEnabledExtensionNames = requiredExtensions.data();
    create_info.enabledLayerCount = requiredValidationLayerName.size();
    create_info.ppEnabledLayerNames = requiredValidationLayerName.data();
    VkResult result = vkCreateInstance(&create_info, allocator, &instance);
    if (result != VK_SUCCESS) {
        LOG_ERROR("vkCreateInstance failed with result: {0}", result);
        return false;
    }

#if defined(_DEBUG)
    LOG_TRACE("Creating Vulkan debugger...");
    uint32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;  

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    debug_create_info.messageSeverity = log_severity;
    debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    debug_create_info.pfnUserCallback = VkDebugCallback;

    PFN_vkCreateDebugUtilsMessengerEXT func =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    VK_CHECK(func(instance, &debug_create_info, allocator, &debugMessenger));
    LOG_INFO("Vulkan debugger created.");
#endif

    LOG_TRACE("Creating Vulkan surface...");
    surface = VK_Plat_CreateSurfaceKHR(instance, windowHandle);
    if (surface == VK_NULL_HANDLE)
    {
        return false;
    }
    LOG_TRACE("Vulkan surface created.");

    if (!device->Create(instance, surface, *allocator))
    {
        LOG_CRITICAL("Device dosen`t create...");
        return false;
    }
    LOG_INFO("Vulkan renderer initialized successfully.");
    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanContext::VkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* user_data)
{
    switch (messageSeverity) {
    default:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        LOG_ERROR(callbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        LOG_WARN(callbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        LOG_INFO(callbackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        LOG_TRACE(callbackData->pMessage);
        break;
    }
    return VK_FALSE;
}
