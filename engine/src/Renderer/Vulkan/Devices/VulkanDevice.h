#pragma once
#include "vulkan\vulkan.h"
#include <Platform\Common\Types.h>
#include <Renderer\Vulkan\VulkanTypes.h>

class VulkanDevice
{
public:

    int32 graphicsQueueIndex;
    int32 presentQueueIndex;
    int32 transferQueueIndex;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VkFormat depthFormat;
	bool Create(VkInstance& instance, VkSurfaceKHR& surface, VkAllocationCallbacks& allocator);
	~VulkanDevice();
    VulkanSwapchainSupportInfo GetVulkanSwapchainSupportInfo();

    void VulkanDeviceQuerySwapchainSupport(
        VkPhysicalDevice physicalDev,
        VkSurfaceKHR surface,
        VulkanSwapchainSupportInfo* outSupportInfo);
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkCommandPool graphicsCommandPool;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
private:
    VulkanSwapchainSupportInfo swapchainSupport;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;



    VkAllocationCallbacks* allocator;
	bool SelectPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);
    bool PhysicalDeviceMeetsRequirements(
        VkPhysicalDevice& device, VkSurfaceKHR& surface, const VkPhysicalDeviceProperties* properties, const VkPhysicalDeviceFeatures* features, VulkanPhysicalDeviceQueueFamilyInfo* queueInfo, const VulkanPhysicalDeviceRequirements* requirements);


    bool DetectDepthFormat();
};
