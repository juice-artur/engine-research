#pragma once
#include "vulkan\vulkan.h"
#include <Platform\Common\Types.h>
#include <Renderer\Vulkan\VulkanTypes.h>

class VulkanDevice
{
public:
	bool Create(VkInstance& instance, VkSurfaceKHR& surface, VkAllocationCallbacks& allocator);
	~VulkanDevice();

private:
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    VulkanSwapchainSupportInfo swapchainSupport;
    int32 graphicsQueueIndex;
    int32 presentQueueIndex;
    int32 transferQueueIndex;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;

    VkAllocationCallbacks* allocator;

	bool SelectPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);
    bool PhysicalDeviceMeetsRequirements(
        VkPhysicalDevice& device, VkSurfaceKHR& surface, const VkPhysicalDeviceProperties* properties, const VkPhysicalDeviceFeatures* features, VulkanPhysicalDeviceQueueFamilyInfo* queueInfo, const VulkanPhysicalDeviceRequirements* requirements);

    void VulkanDeviceQuerySwapchainSupport(
        VkPhysicalDevice physicalDev,
        VkSurfaceKHR surface,
        VulkanSwapchainSupportInfo* outSupportInfo);
};
