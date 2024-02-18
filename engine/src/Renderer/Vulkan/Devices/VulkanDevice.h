#pragma once
#include "vulkan\vulkan.h"
#include <Platform\Common\Types.h>
#include <Renderer\Vulkan\VulkanTypes.h>

class VulkanDevice
{
public:
	bool Create(VkInstance& instance, VkSurfaceKHR& surface);
	~VulkanDevice();

private:
	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice;
    VulkanSwapchainSupportInfo swapchainSupport;
    int32 graphicsQueueIndex;
    int32 presentQueueIndex;
    int32 transferQueueIndex;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
	bool SelectPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);
    bool PhysicalDeviceMeetsRequirements(
        VkPhysicalDevice& device, VkSurfaceKHR& surface, const VkPhysicalDeviceProperties* properties, const VkPhysicalDeviceFeatures* features, VulkanPhysicalDeviceQueueFamilyInfo* queueInfo, const VulkanPhysicalDeviceRequirements* requirements);

    void VulkanDeviceQuerySwapchainSupport(
        VkPhysicalDevice physical_device,
        VkSurfaceKHR surface,
        VulkanSwapchainSupportInfo* outSupportInfo);
};
